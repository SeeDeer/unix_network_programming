#include	"unp.h"
#include	<time.h>

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr,clientAddr;
	char				buff[MAXLINE];
	time_t				ticks;
	socklen_t sockByteLen;
	pid_t pid;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(13);	/* daytime server */

	if ( bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("bind error");

	Listen(listenfd, LISTENQ);

	for ( ; ; ) {
		sockByteLen = sizeof(clientAddr);
		connfd = Accept(listenfd, (SA *)&clientAddr, &sockByteLen);
		printf("connection from %s, port %d\n",
				Inet_ntop(AF_INET, &clientAddr.sin_addr, buff, sizeof(buff)),
				ntohs(clientAddr.sin_port));

		if( (pid = Fork()) == 0){
			Close(listenfd);
			printf("chlid pid:%d\n",getpid());
			// doit(connfd);
			for (int i = 0; i < 3; i++)
			{
				ticks = time(NULL);
				snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
				Write(connfd, buff, strlen(buff));
				sleep(10);
			}

			Close(connfd);
			exit(0);
		}

		Close(connfd);
	}
}
