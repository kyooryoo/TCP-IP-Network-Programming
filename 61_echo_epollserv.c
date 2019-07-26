#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
// 导入epoll库
#include <sys/epoll.h>
#define BUF_SIZE 1024
#define EPOLL_SIZE 1024
void error_handling(char *msg);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    int str_len, i;
    // 准备epoll文件描述符和事件个数记录变量
    int epfd, event_cnt;
    struct sockaddr_in serv_addr, clnt_addr;
    // 
    struct epoll_event *ep_events;
    struct epoll_event event;
    socklen_t addr_size;
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
    
    // 初始化epoll文件描述符和保存变化事件的epoll_event结构体指针
    epfd=epoll_create(EPOLL_SIZE);
    ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

    // 定义监视的读取类型事件，添加服务器套接字到监视范围
    event.events=EPOLLIN;
    event.data.fd=serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    while(1)
    {
        // 对注册的写入类型对象保持监视，直到事件发生
        event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt==-1) {
            puts("epoll_wait() error!");
            break;
        }

        // 对监视得到的事件，逐一开始以下循环操作
        for(i=0; i<event_cnt; i++)
        {
            if(ep_events[i].data.fd==serv_sock)
            {
                // 如果监视得到的写入事件发生在服务器套接字上
                addr_size=sizeof(clnt_addr);
                clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_size);
                event.events=EPOLLIN;
                event.data.fd=clnt_sock;
                // 将客户端套接字加入监视范围
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client: %d \n", clnt_sock);
            }
            else
            {
                // 如果监视得到的写入事件不是发生在服务器套接字上
                str_len=read(ep_events[i].data.fd, msg, BUF_SIZE);
                if(str_len==0) {
                    // 如果
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                    close(ep_events[i].data.fd);
                    printf("closed client: %d \n", ep_events[i].data.fd);
                }
                else {
                    write(ep_events[i].data.fd, msg, str_len);
                }
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}