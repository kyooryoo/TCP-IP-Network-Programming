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
    struct sockaddr_in brd_addr;
    char msg[BUF_SIZE];
    if(argc!=2) {
        printf("Usage : %s <PORT>\n", argv[0]);
        exit(1);
    }

    recv_sock=socket(PF_INET, SOCK_DGRAM, 0);
    memset(&brd_addr, 0, sizeof(brd_addr));
    brd_addr.sin_family=AF_INET;
    brd_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    brd_addr.sin_port=htons(atoi(argv[1]));

    // 绑定作为接收源的多播地址到本地的接收套接字
    if(bind(recv_sock, (struct sockaddr*)&brd_addr, sizeof(brd_addr))==-1)
        error_handling("bind() error!");

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