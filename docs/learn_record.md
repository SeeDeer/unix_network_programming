# 学习笔录

把网络编程当成读写一个文件来看

## 网络端口

- 端口管理机构：[iana.org](https://www.iana.org/assignments/service-names-port-numbers/)

- 49152到65535号端口, 根据定义，该段端口属于“动态端口”范围，没有端口可以被正式地注册占用

- 维基百科：https://zh.wikipedia.org/wiki/TCP/UDP%E7%AB%AF%E5%8F%A3%E5%88%97%E8%A1%A8
- /etc/services文件也是端口记录

| 端口 | 描述 | 状态 |
| ---- | ---| --- |
| 13 | daytime协议，给请求主机发送日期和时间 | 官方 |

## connect

The connect function is used by a TCP client to establish a connection with a TCP
server.

```c
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
// Returns: 0 if OK, -1 on error
```

1. 默认为阻塞型接口,连接未成功时,线程进入睡眠状态;
2. 测试时请把ubuntu虚拟机设置成桥接模式,以模拟真实网络环境

### 错误返回

1. connect error: Operation now in progress
    超时时间设置过短,比如3s

## 疑问解答

1. connect接口超时时间怎么控制 ？
    https://www.cnblogs.com/minglee/p/10161899.html

2. connect接口超时时间设置无效,总是21s
    ```c
    socklen_t optlen = sizeof(timeo);
	timeo.tv_sec = 25;
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, optlen) < 0)
		err_sys("SO_SNDTIMEO setsockopt error");
    ```
    设置数值小于21, 返回错误:connect error: Operation now in progress


## 参考博客

1. https://www.cnblogs.com/lshs/p/6038477.html

