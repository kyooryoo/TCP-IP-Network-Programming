#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 100
void error_handling(char * msg);

int main(int argc, char * argv[])
{
    int send_sock, so_brd=1;
    struct sockaddr_in brd_addr;
    FILE *fp;
    char msg[BUF_SIZE];
    // 这里修改了参数，第三个参数是要发送的文本文件名
    // 与原书硬写入发送文件名的运行方式不同
    if(argc!=4) {
        printf("Usage : %s <GroupIP> <PORT> <FILE>\n", argv[0]);
        exit(1);
    }

    // 注意这里创建的是UDP类型套接字
    send_sock=socket(PF_INET, SOCK_DGRAM, 0);
    memset(&brd_addr, 0, sizeof(brd_addr));
    brd_addr.sin_family=AF_INET;
    brd_addr.sin_addr.s_addr=inet_addr(argv[1]);
    brd_addr.sin_port=htons(atoi(argv[2]));

    // 对于广播，必须要指定允许发送广播类型数据包
    setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, 
        (void*)&so_brd, sizeof(so_brd));
    if((fp=fopen(argv[3], "r"))==NULL)
        error_handling("fopen() error!");

    while(!feof(fp)) {
        fgets(msg, BUF_SIZE, fp);
        sendto(send_sock, msg, strlen(msg), 0, 
            (struct sockaddr*)&brd_addr, sizeof(brd_addr));
        sleep(1);
    }
    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}