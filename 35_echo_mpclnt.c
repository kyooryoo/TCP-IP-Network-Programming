#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    pid_t pid;

    if(argc!=3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error!");
    else
        puts("Connected......");

    pid=fork();
    // 分别为父子进程分配读写操作的函数
    if(pid==0)
        write_routine(sock, message);
    else
        read_routine(sock, message);
    close(sock);
    return 0;
}

void read_routine(int sock, char *message)
{
    while(1)
    {
        int str_len=read(sock, message, BUF_SIZE);
        // 拿到空值后即退出，否则输出结果
        if(str_len==0) return;
        message[str_len]=0;
        printf("Message from server : %s", message);
    }
}

void write_routine(int sock, char *message)
{
    fputs("Input message(Q to quit)\n", stdout);
    while(1)
    {
        fgets(message, BUF_SIZE, stdin);
        // 当用户输入q或Q，程序关闭套接字并返回空值
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
        {
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, message, strlen(message));
    }
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
