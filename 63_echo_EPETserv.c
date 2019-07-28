#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
// 添加包的引用
#include <fcntl.h>
#include <errno.h>
#define BUF_SIZE 4
#define EPOLL_SIZE 1024
// 声明设定非阻塞模式的方法
void setnonblockingmode(int fd);
void error_handling(char *msg);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    int str_len, i;
    int epfd, event_cnt;
    struct sockaddr_in serv_addr, clnt_addr;
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
    
    epfd=epoll_create(EPOLL_SIZE);
    ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

    // 设置服务器套接字为非阻塞模式
    setnonblockingmode(serv_sock);
    event.events=EPOLLIN;
    event.data.fd=serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    while(1)
    {
        event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt==-1) {
            puts("epoll_wait() error!");
            break;
        }
        // 验证调用epoll_wait函数的次数
        puts("return epoll_wait");
        for(i=0; i<event_cnt; i++)
        {
            if(ep_events[i].data.fd==serv_sock)
            {
                addr_size=sizeof(clnt_addr);
                clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_size);
                event.events=EPOLLIN|EPOLLET;
                event.data.fd=clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client: %d \n", clnt_sock);
            }
            else
            {
                // 发生事件时需要读取输入缓冲的所有数据，因此循环调用read函数
                while(1)
                {
                    str_len=read(ep_events[i].data.fd, msg, BUF_SIZE);
                    // 在接受到EOF信息，即读取到的字符串长度为0时退出循环
                    if(str_len==0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                        close(ep_events[i].data.fd);
                        printf("closed client: %d \n", ep_events[i].data.fd);
                        break;
                    }
                    // 在read发生错误，即写入缓存已经为空，且错误信息为EAGAIN时退出循环
                    else if(str_len<0) {
                        if(errno==EAGAIN)
                            break;
                    }
                    // 在读取信息正常时写入客户端套接字
                    else {
                        write(ep_events[i].data.fd, msg, str_len);
                    }
                }
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void setnonblockingmode(int fd) {
    int flag=fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag|O_NONBLOCK);
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
