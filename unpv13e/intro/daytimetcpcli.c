/*****************************************************************
 * @File: unix_network_programming/unpv13e/intro/daytimetcpcli.c
 * @Author: LinusZhao
 * @Date: 2019-08-24 21:30:28
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-08-25 23:40:50
 * @Description: 基于ipv4的tcp客户端程序
 *****************************************************************/

#include	"unp.h"
#include	<netinet/tcp.h>

#ifndef SERVE_IPV4_ADDR
// #define  SERVE_IPV4_ADDR  "139.224.135.102"  // my aliyun_ecs
#define  SERVE_IPV4_ADDR  "192.168.3.1"  // my aliyun_ecs
#endif

long long GetCurrentMSecond(void);

int main(int argc, char **argv)
{
	int					sockfd, n, count = 0;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;
    struct timeval timeo = {3, 0}; 
	if (argc != 2)
		err_quit("usage: a.out <IPaddress>");

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	socklen_t optlen = sizeof(timeo);
	timeo.tv_sec = 25;
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, optlen) < 0)
		err_sys("SO_SNDTIMEO setsockopt error");
	
	optlen = sizeof(timeo);
	if (getsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, &optlen) < 0)
		err_sys("SO_SNDTIMEO getsockopt error");
	printf("socket send timeout = %ld.%ld\n", timeo.tv_sec, timeo.tv_usec*1000);

	// 接口超时时间还是不变,重试次数4次,不变  ??
	int syn_cnt = 10; 
	if (setsockopt(sockfd,IPPROTO_TCP,TCP_SYNCNT,&syn_cnt, sizeof(syn_cnt)) < 0)
		err_sys("TCP_SYNCNT setsockopt error");

	optlen = sizeof(syn_cnt);
	if (getsockopt(sockfd,IPPROTO_TCP,TCP_SYNCNT,&syn_cnt, &optlen) < 0)
		err_sys("TCP_SYNCNT getsockopt error");
	printf("TCP_SYNCNT getsockopt = %d\n", syn_cnt);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(13);	/* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);
		
	long long llBeginTime = GetCurrentMSecond();
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
		long long llEndTime = GetCurrentMSecond();
		// 计算打印重试时间
		printf("connect failed cost time:%lld\n",llEndTime-llBeginTime);
		err_sys("connect error");
	}
	
	while ( (n = read(sockfd, recvline, 10)) > 0) {
		count++;
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	printf("read count:%d\n",count);
	if (n < 0)
		err_sys("read error");

	exit(0);
}

long long GetCurrentMSecond(void)
{
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
