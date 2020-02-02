/*****************************************************************
 * @File: arp
 * @Author: LinusZhao
 * @Date: 2019-10-24 20:07:33
 * @LastEditors: LinusZhao
 * @LastEditTime: 2020-02-01 16:33:14
 * @Description: arp地址解析协议的简单实现，哎，我要组包，这谁的数据包，把mac地址给我 ！
 *****************************************************************/

#include "arp.h"

// const items
const char INTERFACE[] = "ens33";
const uint8_t TargetMac[] = {0xff,0xff,0xff,0xff,0xff,0xff};

/*****************************************************************
 * @Function: send_arp_packet
 * @Description: 组成一个arp包发送
 * @Param[in]: srcDevAddr,发送arp包设备的mac地址和ip地址
 * @Param[in]: destDevAddr,接收arp包设备的mac地址和ip地址
 * @Param[in]: uint16_t opcode, ARPOP_REQUEST or ARPOP_REPLY
 * @Return: void
 *****************************************************************/
int send_arp_packet(DEV_ADDR_T *srcDevAddr,DEV_ADDR_T *destDevAddr,uint16_t opcode)
{
    int arp_fd;
    uint8_t frame[42];
    
    arp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(arp_fd < 0) {
        LOG_ERR("socket() failed");
        return arp_fd;
        exit(1);
    }
 
    // create arp frame -- 构建arp帧
    struct ether_header eth_head;
    struct ether_arp eth_arp;
    memcpy(eth_head.ether_dhost, destDevAddr->hw_addr, ETHER_ADDR_LEN);
    memcpy(eth_head.ether_shost, srcDevAddr->hw_addr, ETHER_ADDR_LEN);
    eth_head.ether_type = htons(ETHERTYPE_ARP);
 
    eth_arp.arp_hrd = htons(ARPHRD_ETHER);
    eth_arp.arp_pro = htons(ETHERTYPE_IP);
    eth_arp.arp_hln = ETHER_ADDR_LEN;
    eth_arp.arp_pln = 4;
    eth_arp.arp_op = htons(opcode); // request or reply
    memcpy(eth_arp.arp_sha, srcDevAddr->hw_addr, ETHER_ADDR_LEN);
    memcpy(eth_arp.arp_spa, srcDevAddr->ip_addr, 4);
    memcpy(eth_arp.arp_tha, destDevAddr->hw_addr, ETHER_ADDR_LEN);
    memcpy(eth_arp.arp_tpa, destDevAddr->ip_addr, 4);
 
    // 组包复制到frame数组
    memcpy(frame, &eth_head, sizeof(eth_head));
    memcpy(frame + sizeof(eth_head), &eth_arp, sizeof(eth_arp));
 
    // make sockaddr_ll ready for sendto() function -- 用作sendto()函数中的参数，此处主要是指定一个网络接口
    struct sockaddr_ll destaddr;
    destaddr.sll_family = AF_PACKET;
    if((destaddr.sll_ifindex = if_nametoindex(INTERFACE)) == 0) {
        LOG_ERR("if_nametoindex() failed");
        return -1;
        exit(1);
    }
    destaddr.sll_halen = htons(ETHER_ADDR_LEN);
    if(sendto(arp_fd, frame, sizeof(frame), 0, (struct sockaddr *)&destaddr, sizeof(destaddr)) == -1) {
        LOG_ERR("sendto() failed");
        return -1;
        exit(1);
    }
    
    close(arp_fd);

    return 0;
}


