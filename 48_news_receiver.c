#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 100
void error_handling(char * msg);

int main(int argc, char * argv[])
{
    int recv_sock, str_len;
    struct sockaddr_in mul_addr;
    struct ip_mreq join_addr;
    char msg[BUF_SIZE];
    if(argc!=3) {
        printf("Usage : %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }

    recv_sock=socket(PF_INET, SOCK_DGRAM, 0);
    memset(&mul_addr, 0, sizeof(mul_addr));
    mul_addr.sin_family=AF_INET;
    mul_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    mul_addr.sin_port=htons(atoi(argv[2]));

    // 绑定作为接收源的多播地址到本地的接收套接字
    if(bind(recv_sock, (struct sockaddr*)&mul_addr, sizeof(mul_addr))==-1)
        error_handling("bind() error!");

    // 准备定义多播地址的结构体，包含多播组地址和本地主机地址
    join_addr.imr_multiaddr.s_addr=inet_addr(argv[1]);
    join_addr.imr_interface.s_addr=htonl(INADDR_ANY);

    // 设置本地套接字选项，使用之前的多播地址结构类变量
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
        (void*)&join_addr, sizeof(join_addr));

    while(1) {
        str_len=recvfrom(recv_sock, msg, BUF_SIZE-1, 0, NULL, 0);
        if(str_len<0)
            break;
        msg[str_len]=0;
        fputs(msg, stdout);
    }
    close(recv_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}