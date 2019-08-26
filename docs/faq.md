# 常见问题

### Descriptor Reference Counts

描述符引用计数，每个文件或者套接字被打开时，内核会返回给进程一个描述符fd，这个fd还有个引用计数，由内核在文件表项中维护，真正的释放资源是该引用计数为0时发生，close(fd)只是把引用计数减一了。

```c
	.......
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
			doit(connfd);
			Close(connfd);
			exit(0);
		}
		Close(connfd);
	}
```

​	如上代码片段，连接套接字connfd在父进程中被关闭，却没有终止与客户端的连接呢？

fork返回后，connfd描述符已经在父子进程共享了，也就是被“复制”，connfd的引用计数值变为2，这样父进程close只不过是将引用计数减一；

代码实现功能：父进程负责监听客户端的连接请求；每有一个连接建立时就fork一个子进程，由进程处理客户端数据请求doit(connfd)；这里有个小问题，子进程exit(0)后，父进程没有回收处理，将会成为僵尸进程(Z)