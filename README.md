# UNIX Network Programming

感谢 W. Richard Stevens编辑的经典书籍 

http://www.kohala.com/start/

http://www.unpbook.com/

docs/UNIX Network Programming Volume.1.3rd.Ed 英文书籍原版

unpv13e.tar.gz 为书籍源码

## 快速入门
参考文件 unpv13e/README

## 常见问题

1. 编译错误
```shell
inet_ntop.c: In function ‘inet_ntop’:
inet_ntop.c:60:9: error: argument ‘size’ doesn’t match prototype
  size_t size;
         ^
In file included from inet_ntop.c:27:0:
/usr/include/arpa/inet.h:64:20: error: prototype declaration
 extern const char *inet_ntop (int __af, const void *__restrict __cp,
                    ^
<内置>: recipe for target 'inet_ntop.o' failed
make: *** [inet_ntop.o] Error 1
```
**解决方法**
unpv13e/libfree/inet_ntop.c 文件中所有头文件新增如下宏定义
#define size_t socklen_t