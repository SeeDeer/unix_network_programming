/*****************************************************************
 * @File: main.c
 * @Author: LinusZhao
 * @Date: 2019-10-22 23:52:31
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-10-23 01:23:26
 * @Description: 实现一个小型tcp服务器的功能,可并发接受10个客户端前来连接请求数据
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

int main(int argc, char *argv[])
{
    int listenfd,connectfd;
    struct sockaddr_in serve_addr, client_addr;
    socklen_t sock_byte_len;
    char buf[1024];

    pid_t pid;
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if (listenfd < 0){
        printf("socket open error! \r\n");
        return -1;
    }
    
    bzero(&serve_addr,sizeof(serve_addr));
    serve_addr.sin_family = AF_INET;
    serve_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serve_addr.sin_port = htons(13);

    if( bind(listenfd, (struct sockaddr *)&serve_addr, sizeof(serve_addr)) < 0){
        printf("bind error! \r\n");
        return -1;
    }

    if( listen(listenfd,1024) < 0){
        printf("listen error! \r\n");
        return -1;
    }

    while (1){
        sock_byte_len = sizeof(client_addr);
        connectfd = accept(listenfd,(struct sockaddr *)&client_addr,&sock_byte_len);
        printf("connection from %s, port %d\n",\
                    inet_ntop(AF_INET,&client_addr.sin_addr,buf,sizeof(client_addr)),\
                    ntohs(client_addr.sin_port));

        pid = fork();
        if( pid < 0 ){
            printf("fork error! \r\n");
            return -1;
        }

        if (pid == 0){
            close(listenfd);
            // 处理客户端请求,往connectfd写数据
            time_t ticks;
            for (int i = 0; i < 3; i++){
				ticks = time(NULL);
				snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
				write(connectfd, buf, strlen(buf));
				sleep(10);
			}
            close(connectfd);
            exit(0);
        }

        close(connectfd);
        
    }
    
    return 0;
}