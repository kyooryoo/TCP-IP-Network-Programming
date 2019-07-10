// 回声测试的UDP客户端
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int sock;
    char message[BUF_SIZE];
    struct sockaddr_in a_addr, b_addr;
    socklen_t addr_size;
    int str_len, i;

    if(argc!=2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    sock=socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handling("socket() error");

    memset(&a_addr, 0, sizeof(a_addr));
    a_addr.sin_family=AF_INET;
    a_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    a_addr.sin_port=htons(atoi(argv[1]));

    if(bind(sock, (struct sockaddr*)&a_addr, sizeof(a_addr))==-1)
        error_handling("bind() error!");

    for(i=0; i<3; i++)
    {
        sleep(5);
        addr_size=sizeof(b_addr);
        str_len=recvfrom(sock, message, BUF_SIZE, 0,
            (struct sockaddr*)&b_addr, &addr_size);
        printf("Message %d: %s \n", i+1, message);
    }
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
