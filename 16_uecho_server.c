// 回声测试的UDP客户端
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int serv_sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;

    if(argc!=2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // 注意不同于TCP的套机字，这里运送SOCK_DGRAM
    serv_sock=socket(PF_INET, SOCK_DGRAM, 0);
    if(serv_sock == -1)
        error_handling("UDP socket creation error!");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    // 建立连接并分配地址
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error!");

    while(1)
    {
        clnt_addr_size=sizeof(clnt_addr);
        // 使用之前分配的地址接收数据，不限制数据传输对象
        str_len=recvfrom(serv_sock, message, BUF_SIZE, 0,
            (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        // 使用之前获得的客户端地址作为接收方返回接收的数据
        sendto(serv_sock, message, str_len, 0,
            (struct sockaddr*)&clnt_addr, clnt_addr_size);
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
