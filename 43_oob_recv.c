#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#define BUF_SIZE 30
void error_handling(char *msg);
void urg_handler(int signo);

// 声明客户端负责发送信息的套接字
int send_sock;
// 声明服务器端接收信息的套接字
int recv_sock;

int main(int argc, char *argv[])
{
    int str_len, state;
    struct sockaddr_in recv_addr, send_addr;
    struct sigaction act;
    socklen_t send_addr_size;
    char msg[BUF_SIZE];

    if(argc!=2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 为触发器注册紧急信号处理的专属函数
    act.sa_handler=urg_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;

    // 定义服务器端接收信息的套接字并初始化地址及端口
    recv_sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family=AF_INET;
    recv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    recv_addr.sin_port=htons(atoi(argv[1]));

    // 绑定并对服务器端接收信息的套接字做监听
    if(bind(recv_sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr))==-1)
        error_handling("bind() error!");
    listen(recv_sock, 5);

    // 服务器端接受来自客户端信息并初始化本地的客户端套接字
    send_addr_size=sizeof(send_addr);
    send_sock=accept(recv_sock, (struct sockaddr*)&send_addr, &send_addr_size);

    // 将recv_sock的拥有者指定为getpid()函数返回的进程
    fcntl(send_sock, F_SETOWN, getpid());
    // 在系统发现紧急信号时调用触发器
    state=sigaction(SIGURG, &act, 0);

    while((str_len=recv(send_sock, msg, sizeof(msg)-1, 0))!=0) {
        if(str_len==-1)
            continue;
        msg[str_len]=0;
        puts(msg);
    }
    close(send_sock);    
    close(recv_sock);
    return 0;
}

void urg_handler(int signo) {
    int str_len;
    char msg[BUF_SIZE];
    // 调用了接收紧急信号处理的函数
    str_len=recv(send_sock, msg, sizeof(msg)-1, MSG_OOB);
    msg[str_len]=0;
    printf("Urgent message: %s \n", msg);
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}