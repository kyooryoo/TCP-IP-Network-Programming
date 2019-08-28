#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#define BUF_SIZE 1024
#define MAX_CLNT 256
void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

// 用来记录客户端数量的变量和保存客户端套接字的数组
// 访问这两个全局变量的代码将会成为程序的关键部分
int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size=sizeof(clnt_addr);
    pthread_t t_id;
    char msg[BUF_SIZE];

    if(argc!=2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error!");
    if(listen(serv_sock, 5)==-1)
        error_handling("listen() error!");
    
    while(1)
    {
        // 接受客户端连接请求后即将其加入客户端套接字数组
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        // 涉及到临界区的全局变量，都需要先用互斥量将其锁定
        pthread_mutex_lock(&mutx);
        // 新的客户端套接字会写入套接字数组
        clnt_socks[clnt_cnt++]=clnt_sock;
        pthread_mutex_unlock(&mutx);

        // 创建新的线程用于向新加入的客户端提供服务，调用单独定义的函数
        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        // 销毁进程的函数调用后就会伺机而动了么？不会提前销毁未完成的进程么？
        pthread_detach(t_id);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_addr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg)
{
    int clnt_sock=*((int*)arg);
    int str_len=0, i;
    char msg[BUF_SIZE];

    while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
        send_msg(msg, str_len);
    // 以下使用互斥量锁定的关键区域用来删除离开的客户端套接字
    pthread_mutex_lock(&mutx);
    for(i=0; i<clnt_cnt; i++) {
        if(clnt_sock==clnt_socks[i]) {
            while(i++<clnt_cnt-1)
                clnt_socks[i]=clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    
    close(clnt_sock);
    return NULL;
}

void send_msg(char *msg, int len) {
    int i;
    // 使用互斥量锁定关键区域，即全局变量的套接字数组
    // 向所有连接的客户端发送信息
    pthread_mutex_lock(&mutx);
    for(i=0; i<clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}