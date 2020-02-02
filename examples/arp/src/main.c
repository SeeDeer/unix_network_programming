/*****************************************************************
 * @File: Do not edit
 * @Author: LinusZhao
 * @Date: 2019-10-24 15:38:10
 * @LastEditors: LinusZhao
 * @LastEditTime: 2020-02-01 20:13:50
 * @Description: 实现一个arp广播包的发送 & arp的响应
 *****************************************************************/
#include <pthread.h>
#include "arp.h"

DEV_ADDR_T SrcDevAddr = {
    {0x00,0x0c,0x29,0x01,0x67,0xcb},    // ubuntu mac
    {192,168,1,8}                       // ubuntu ip
};

DEV_ADDR_T DestDevAddr = {
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
    {192,168,1,1}
};

ARP_TAB_T ArpTabs[5];

void *thr_fn2(void *arg)
{
    int ret = 0;
    int count = 0;
    while (1){
        send_arp_packet(&SrcDevAddr,&DestDevAddr,ARPOP_REQUEST);
        usleep(200*1000); //200ms
        count++;
        if(count == 15){
            DestDevAddr.ip_addr[3]++;
            count = 0;
        }
    }
    
	printf("thread 2 returning\n");
	return((void *)2);
}

void *thr_fn1(void *arg)
{
    uint8_t recvbuf[1024];
    uint8_t	controlbuf[100];
    int ret = 0,count = 0;
    char previousIpAddr[16]={"255.255.255.0"},tmpIpAddr[16];

    struct sockaddr_ll destaddr;
    destaddr.sll_family = AF_PACKET;
    if((destaddr.sll_ifindex = if_nametoindex("ens33")) == 0) {
        LOG_ERR("if_nametoindex() failed");
        return (void *)-1;
    }
    destaddr.sll_halen = htons(ETHER_ADDR_LEN);
    
    int rev_arp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(rev_arp_fd < 0) {
        LOG_ERR("socket rev_arp_fd failed");
        return (void *)-2;
    }

    struct msghdr arp_reply_msg;
    struct iovec  iov;
    iov.iov_base = recvbuf;
	iov.iov_len = sizeof(recvbuf);
    arp_reply_msg.msg_name    = (struct sockaddr *)&destaddr;
    arp_reply_msg.msg_iov     = &iov;
    arp_reply_msg.msg_iovlen  = 1; // 为啥是 1 ？
    arp_reply_msg.msg_control = controlbuf;

    while(1){
		arp_reply_msg.msg_namelen = sizeof(destaddr);
		arp_reply_msg.msg_controllen = sizeof(controlbuf);
        int rev_num;
		rev_num = recvmsg(rev_arp_fd, &arp_reply_msg, 0);
		if (rev_num < 0) {
			if (errno == EINTR)
				continue;
			else
				LOG_ERR("recvmsg error");
		}
        // LOG_DEBUG("arp_reply_msg:%d\n",rev_num);
        int i = 0;
        // for(i = 0; i < rev_num; i++){
        //     printf("%02x ",recvbuf[i]);
        // }
        struct ether_header *eth_head_p = (struct ether_header *)recvbuf;
        struct ether_arp *eth_arp_p = (struct ether_arp *)(recvbuf + sizeof(struct ether_header));
        if(eth_arp_p->arp_op == htons(ARPOP_REPLY)){
            snprintf(tmpIpAddr,16,"%d.%d.%d.%d",
                            eth_arp_p->arp_spa[0],eth_arp_p->arp_spa[1],\
                            eth_arp_p->arp_spa[2],eth_arp_p->arp_spa[3]);
            if(strcmp(previousIpAddr,tmpIpAddr) != 0){
                strcpy(ArpTabs[count].ip_addr,tmpIpAddr);
                memcpy(ArpTabs[count].hw_addr,eth_arp_p->arp_sha,ETHER_ADDR_LEN);
                LOG_NOTICE("%02x:%02x:%02x:%02x:%02x:%02x ==> %s",
                    ArpTabs[count].hw_addr[0],ArpTabs[count].hw_addr[1],ArpTabs[count].hw_addr[2],\
                    ArpTabs[count].hw_addr[3],ArpTabs[count].hw_addr[4],ArpTabs[count].hw_addr[5],\
                    ArpTabs[count].ip_addr);
                strcpy(previousIpAddr,tmpIpAddr);
                count++;
            }
            if(sizeof(ArpTabs)/sizeof(ArpTabs[0]) == count){
                printf("thread 1 exiting\n");
                pthread_exit((void *)1);
            }
        }
	}
}

int main(int argc, char **argv) 
{
    int ret = 0;
    void *tret;
    
    pthread_t thr_1, thr_2;
    ret = pthread_create(&thr_1,NULL,thr_fn1,NULL);
    if (ret != 0){
        LOG_ERR("pthread_create fail , ret:%d",ret);
        exit(1);
    }
    LOG_NOTICE("pthread_create thr_fn1 success.");
    ret = pthread_create(&thr_2,NULL,thr_fn2,NULL);
    if (ret != 0){
        LOG_ERR("pthread_create fail , ret:%d",ret);
        exit(1);
    }
    LOG_NOTICE("pthread_create thr_fn2 success.");

    ret = pthread_join(thr_1, &tret);
	if (ret != 0){
        LOG_ERR("can't join with thread 1");
    }
	LOG_NOTICE("thread 1 exit code %ld\n", (long)tret);

	ret = pthread_join(thr_2, &tret);
	if (ret != 0){
        LOG_ERR("can't join with thread 2");
    }
	LOG_NOTICE("thread 2 exit code %ld\n", (long)tret);
 
    return 0;
}
