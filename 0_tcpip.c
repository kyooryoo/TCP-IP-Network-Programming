// 创建和配置服务器端套接字用于响应请求
// 以下操作都在成功时返回0，失败时返回-1
#include <sys/socket.h>
// 创建套接字
// domain：套接字中使用过的协议族（Protocol Family）信息
// type：套接字数据传输类型
// protocol：协议信息
int socket(int domain, int type, int protocol);
// 分配IP地址和端口号
int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen);
// 设置套接字为接受连接的状态
int listen(int sockfd, int backlog);
// 接受连接请求
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

// 创建客户端套接字用来向服务器发送连接请求
#include <sys/socket.h>
int connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen);

/////

// Linux中打开文件或套接字的函数
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// 成功返回文件描述符fd，失败返回-1
// path：文件名的字符串地址
// flag：文件打开模式
int open(const char *path, int flag);

/////

// 关闭文件或套接字
#include <unistd.h>

// 成功返回0，失败返回-1
// fd：（需要关闭的文件的）文件描述符
int close(int fd);

/////

// 写入文件
#include <unistd.h>

// 成功返回写入的字节数，失败返回-1
// fd：传输对象的文件描述符
// buf：保存传输数据的缓冲地址值
// nbytes：需要传输的数据字节数
ssize_t write(int fd, const void * buf, size_t nbytes);

// 元数据类型（primitive）
// 在sys/types.h头文件中由typedef声明定义了一些基本数据类型
// 操作系统为与程序员定义的数据类型加以区分，在自定义类型后添加`_t`
// size_t是typedef声明定义的unsigned int类型
// ssize_t是typedef声明定义的signed int类型

// 读取文件
#include <unistd.h>

// 成功返回接收的字节数（遇到文件结尾返回0），失败返回-1
// 参数解释见写入文件的函数说明
ssize_t read(int fd, void * buf, size_t nbytes);