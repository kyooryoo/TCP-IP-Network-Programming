#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define BUF_SIZE 1024
void error_handling(char *msg);

int main(int argc, char *argv[])
{
    int acpt_sock, recv_sock;
    int str_len, state;
    struct sockaddr_in acpt_addr, recv_addr;
    socklen_t recv_addr_size;
    char msg[BUF_SIZE];

    if(argc!=2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    acpt_sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&acpt_addr, 0, sizeof(acpt_addr));
    acpt_addr.sin_family=AF_INET;
    acpt_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    acpt_addr.sin_port=htons(atoi(argv[1]));

    if(bind(acpt_sock, (struct sockaddr*)&acpt_addr, sizeof(acpt_addr))==-1)
        error_handling("bind() error!");
    listen(acpt_sock, 5);

    recv_addr_size=sizeof(recv_addr);
    recv_sock=accept(acpt_sock, (struct sockaddr*)&recv_addr, &recv_addr_size);
    
    while(1) {
        str_len=recv(recv_sock, msg, sizeof(msg)-1, MSG_PEEK|MSG_DONTWAIT);
        if(str_len>0)
            break;
    }

    msg[str_len]=0;
    printf("recv() with MSG_PEEK got %d bytes: %s \n", str_len, msg);

    str_len=recv(recv_sock, msg, sizeof(msg)-1, 0);
    msg[str_len]=0;
    printf("recv() without MSG_PEEK: %s \n", msg);

    if((str_len=recv(recv_sock, msg, sizeof(msg)-1, 0))==-1)
        error_handling("recv() error!");
    else 
    {
        if(str_len==0)
            puts("the read buf is empty now!");
        else
        {
            msg[str_len]=0;
            printf("still in read buf: %s \n", msg);
        }
    }

    close(acpt_sock);    
    close(recv_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}