// 测试半连接文件传输的客户端
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
    FILE *fp;
    char buf[BUF_SIZE];
    int read_cnt;
    struct sockaddr_in serv_addr;

    if(argc!=3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // 创建文件以便接收传入的数据
    fp=fopen("22_receive.dat", "wb");
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // 接收数据并保存到之前创建的文件，直到接收到EOF
    while((read_cnt=read(sock, buf, BUF_SIZE))!=0)
        fwrite((void*)buf, 1, read_cnt, fp);

    puts("Received file data!");
    // 发送最后的提示消息，如果服务器端未关闭输入流则可以接收
    // 以下的第三个参数原文用的是`10`，会造成服务器端接收到多余字符
    // 修改为`BUF_SIZE`可以修正此问题
    write(sock, "Thank you!", BUF_SIZE);
    fclose(fp);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
