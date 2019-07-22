#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
void error_handling(char *msg);

int main(int argc, char *argv[])
{
    int recv_sock;
    struct sockaddr_in recv_addr;
    if(argc!=4) {
        printf("Usage: %s <IP> <port> <msg>\n", argv[0]);
        exit(1);
    }

    recv_sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family=AF_INET;
    recv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    recv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(recv_sock, (struct sockaddr*)&recv_addr, sizeof(recv_addr))==-1)
        error_handling("connect() error!");

    write(recv_sock, argv[3], strlen(argv[3]));
    close(recv_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}