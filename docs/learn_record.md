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

## 疑问解答

1. connect接口超时时间怎么控制 ？
    https://www.cnblogs.com/minglee/p/10161899.html

