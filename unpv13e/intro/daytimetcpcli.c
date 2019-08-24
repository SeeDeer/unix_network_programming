/*****************************************************************
 * @File: unix_network_programming/unpv13e/intro/daytimetcpcli.c
 * @Author: LinusZhao
 * @Date: 2019-08-24 21:30:28
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-08-24 22:20:12
 * @Description: 基于ipv4的tcp客户端程序
 *****************************************************************/

#include	"unp.h"

#ifndef SERVE_IPV4_ADDR
#define  SERVE_IPV4_ADDR  "139.224.135.102"  // my aliyun_ecs
#endif

long long GetCurrentMSecond(void);

int main(int argc, char **argv)
{
	int					sockfd, n, count = 0;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;

	// if (argc != 2)
	// 	err_quit("usage: a.out <IPaddress>");
	// printf("serv")

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(9999);	/* daytime server */
	if (inet_pton(AF_INET, SERVE_IPV4_ADDR, &servaddr.sin_addr) <= 0)
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
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
