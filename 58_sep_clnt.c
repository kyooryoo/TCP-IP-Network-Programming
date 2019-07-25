#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
void error_handling(char *msg);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char msg[BUF_SIZE];

    FILE *readfp;
    FILE *writefp;

    if(argc!=3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    readfp=fdopen(sock, "r");
    writefp=fdopen(sock, "w");

    while(1)
    {
        // 在接收到EOF标志后，输出接收的信息到标准输出
        if(fgets(msg, sizeof(msg), readfp)==NULL)
            break;
        fputs(msg, stdout);
        fflush(stdout);        
    }

    // 向服务器返回一个信息，验证服务器方的半关闭操作是否有效
    fputs("FROM CLIENT: Thank you! \n", writefp);
    fflush(writefp);
    fclose(writefp);
    fclose(readfp);
    return 0;
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}