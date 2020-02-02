#ifndef _ARP_H
#define _ARP_H

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
#include <netinet/in.h>
#include <netinet/ip.h>
#include "iot_log.h"

typedef struct dev_addr
{
    uint8_t hw_addr[6];
    uint8_t ip_addr[4];
}DEV_ADDR_T;

typedef struct arp_tab
{
    char    ip_addr[16];
    uint8_t hw_addr[6];
}ARP_TAB_T;

/*****************************************************************
 * @Function: send_arp_packet
 * @Description: 组成一个arp包发送
 * @Param[in]: srcDevAddr,发送arp包设备的mac地址和ip地址
 * @Param[in]: destDevAddr,接收arp包设备的mac地址和ip地址
 * @Param[in]: uint16_t opcode, ARPOP_REQUEST or ARPOP_REPLY
 * @Return: void
 *****************************************************************/
int send_arp_packet(DEV_ADDR_T *srcDevAddr,DEV_ADDR_T *destDevAddr,uint16_t opcode);


#endif
