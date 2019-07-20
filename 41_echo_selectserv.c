#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 1024
void error_handling(char *msg);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    struct timeval timeout;
    fd_set reads, cpy_reads;
    socklen_t addr_size;
    int fd_max, str_len, fd_num, i;
    char msg[BUF_SIZE];
    if(argc!=2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error!");
    if(listen(serv_sock, 5)==-1)
        error_handling("listen() error!");
    
    FD_ZERO(&reads);
    // 注册服务器套接字到文件描述符数组
    FD_SET(serv_sock, &reads);
    fd_max=serv_sock;

    while(1)
    {
        cpy_reads=reads;
        timeout.tv_sec=5;
        timeout.tv_usec=5000;

        // 放置包含服务器套接字的文件描述符数组放到第二个参数，只监视读取
        if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)
            break;
        if(fd_num==0)
            continue;

        for(i=0; i<fd_max+1; i++)
        {
            // 与上层循环一起作用，检测发生状态变化的文件描述符
            if(FD_ISSET(i, &cpy_reads))
            {
                // 发生状态变化时检查是否为服务器套接字
                if(i==serv_sock)
                {
                    // 如果是服务器套接字有变化，则接收连接请求
                    addr_size=sizeof(clnt_addr);
                    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_size);
                    // 注册客户端文件描述符到监视读取的文件描述符数组对象
                    FD_SET(clnt_sock, &reads);
                    // 更新监视范围到服务器和所有客户端
                    if(fd_max<clnt_sock)
                        fd_max=clnt_sock;
                    printf("connected client: %d \n", clnt_sock);
                }
                else
                {
                    // 发生变化的不是服务器端套接字，则准备接收数据
                    str_len=read(i, msg, BUF_SIZE);
                    // 判断传入的数据是否为表示断开连接的EOF
                    if(str_len==0)
                    {
                        // 如果是EOF则清除客户端套接字注册并关闭
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d \n", i);
                    }
                    else
                        // 收到的信息为正常字符串则写入客户端套接字
                        write(i, msg, str_len);
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}