#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
// 设置运算结果的字节数为常数
#define RLT_SIZE 4
// 设置待运算数字的字节数为常数
#define OPSZ 4
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    // 为收发数据准备内存空间
    char opmsg[BUF_SIZE];
    int result, opnd_cnt, i;

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

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error!");
    else
        puts("Connected......");

    fputs("Operand count: ", stdout);
    // 从用户输入得到待计算的数字个数
    scanf("%d", &opnd_cnt);
    // 将待计算的数字个数强制转换为字符型并存入数组
    opmsg[0]=(char)opnd_cnt;

    // 从用户输入得到待计算的整数并保存到数组
    for(i=0; i<opnd_cnt; i++)
    {
        printf("Operand %d: ", i+1);
        // 以4字节为单位保存待计算数字，转换为整型指针
        scanf("%d", (int*)&opmsg[i*OPSZ+1]);
    }
    // 删掉缓冲中的字符`\n`
    fgetc(stdin);
    fputs("Operator: ", stdout);
    // 将用户输入的运算符保存到数组
    scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);
    // 传输数组中的运算信息到服务器
    write(sock, opmsg, opnd_cnt*OPSZ+2);
    // 保存服务器返回的运算结果
    read(sock, &result, RLT_SIZE);
    
    printf("Operation result: %d \n", result);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
