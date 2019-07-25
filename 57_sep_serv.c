#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
void error_handling(char *msg);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    FILE *readfp;
    FILE *writefp;

    if(argc!=2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size=sizeof(clnt_addr);
    char msg[BUF_SIZE]={0,};

    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(serv_sock, 5);
    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    readfp=fdopen(clnt_sock, "r");
    writefp=fdopen(clnt_sock, "w");

    // 向客户端发送三条信息，并手动触发发送
    fputs("FROM SERVER: Hi~ client? \n", writefp);
    fputs("I love all of the world \n", writefp);
    fputs("You are awesome! \n", writefp);
    fflush(writefp);

    // 模拟半关闭写入操作的FILE结构体
    fclose(writefp);
    // 读取客户端最后发送的信息，输出到标准输出
    fgets(msg, sizeof(msg), readfp);
    fputs(msg, stdout);
    // 最后关闭读取操作的FILE结构体
    fclose(readfp);
    return 0;
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}