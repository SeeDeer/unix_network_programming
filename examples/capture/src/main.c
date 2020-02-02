/*****************************************************************
 * @File: examples/capture/src/main.c
 * @Author: LinusZhao
 * @Date: 2020-02-01 20:15:52
 * @Company: Tuya
 * @LastEditors: LinusZhao
 * @LastEditTime: 2020-02-01 20:22:27
 * @Description: Do not edit
 *****************************************************************/

#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>

int main(int argc, char **argv)
{
    int sock, n;
    char buffer[2048];
    struct ethhdr *eth;
    struct iphdr *iph;

    sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if(sock < 0){
        perror("socket");
        exit(1);
    }

    while (1) {
        printf("=====================================\n");
        //注意：在这之前我没有调用bind函数，原因是什么呢？
        n = recvfrom(sock,buffer,2048,0,NULL,NULL);

        if(!strcmp(inet_ntoa(src_addr),"192.168.0.103")){
            printf("%d bytes read\n",n);

            //接收到的数据帧头6字节是目的MAC地址，紧接着6字节是源MAC地址。
            eth=(struct ethhdr*)buffer;
            printf("Dest MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
            printf("Source MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);

            iph=(struct iphdr*)(buffer+sizeof(struct ethhdr));
            //我们只对IPV4且没有选项字段的IPv4报文感兴趣
            struct in_addr src_addr, dst_addr;
            memcpy(&src_addr,&iph->saddr,sizeof(struct in_addr));
            memcpy(&dst_addr,&iph->daddr,sizeof(struct in_addr));
            
            if(iph->version ==4 && iph->ihl == 5){
                printf("Source host:%s\n",inet_ntoa(src_addr));
                printf("Dest host:%s\n",inet_ntoa(src_addr));
            }
        }
    }
}