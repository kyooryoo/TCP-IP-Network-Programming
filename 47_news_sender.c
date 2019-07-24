#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define TTL 64
#define BUF_SIZE 100
void error_handling(char * msg);

int main(int argc, char * argv[])
{
    int send_sock;
    struct sockaddr_in mul_addr;
    int time_live=TTL;
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
    memset(&mul_addr, 0, sizeof(mul_addr));
    mul_addr.sin_family=AF_INET;
    mul_addr.sin_addr.s_addr=inet_addr(argv[1]);
    mul_addr.sin_port=htons(atoi(argv[2]));

    // 对于多播，必须要指定套接字数据包的生存时间
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, 
        (void*)&time_live, sizeof(time_live));
    if((fp=fopen(argv[3], "r"))==NULL)
        error_handling("fopen() error!");

    while(!feof(fp)) {
        fgets(msg, BUF_SIZE, fp);
        sendto(send_sock, msg, strlen(msg), 0, 
            (struct sockaddr*)&mul_addr, sizeof(mul_addr));
        sleep(2);
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