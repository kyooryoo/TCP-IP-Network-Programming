// 本程序基于客户端程序hello_client.c
// 逐字节读取数据，验证tcp套接字不存在数据边界
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    // 这里需要更新，做初始化
    int str_len=0;
    // 以下一行语句新添加，用于定位读取字节位置
    int idx=0, read_len=0;


    if(argc!=3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // 创建套接字
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    // 想服务器端发送连接请求
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error!");

    // 以下为原始client程序使用的读取函数
    // str_len=read(sock, message, sizeof(message)-1);
    // if(str_len==-1)
    //    error_handling("read() error!");

    // 以下为本次更新添加的逐字节读取函数
    // while的判断条件与原书中不太一样，更新为修正bug
    // 原书为：while(read_len=read(sock, &message[idx++], 1))
    while((read_len=read(sock, &message[idx++], 1))!=0)
    {
        if(read_len==-1)
            error_handling("read() error!");
        str_len+=read_len;
    }

    printf("Message from server : %s \n", message);
    // 添加显示函数读取次数的语句如下
    printf("Function read call count: %d \n", str_len);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
