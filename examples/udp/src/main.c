/*****************************************************************
 * @File: udp/main.c
 * @Author: LinusZhao
 * @Date: 2019-10-24 21:00:59
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-10-24 22:03:28
 * @Description: Do not edit
 *****************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "iot_log.h"

// 实现一个udp包的发送

int main(int argc, char *argv[])
{
    if (argc != 2){
        printf("usage: udpcli <IPaddress>");
        exit(1);
    }
		
    int udp_sockfd;
    udp_sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(udp_sockfd < 0){
        LOG_ERR("udp_sockfd create fail");
    }

    // 定义接收方地址
    struct sockaddr_in dest_addr;
    memset(&dest_addr,0,sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    if ( inet_pton(AF_INET,argv[1],&dest_addr.sin_addr) < 0){
        LOG_ERR("inet_pton");
        exit(1);
    }
    dest_addr.sin_port = htons(67);
    
    unsigned char sendbuf[10] = {"AI & IoT"};
    unsigned short send_len = strlen(sendbuf);

    int i;
    for (i = 0; i < 100; i++){
        if ( sendto(udp_sockfd, sendbuf, send_len, 0, \
                (struct sockaddr *)&dest_addr, sizeof(dest_addr)) !=  send_len){
                LOG_ERR("sendto");
        }
        sleep(1);
    }

    close(udp_sockfd);

    return 0;
}