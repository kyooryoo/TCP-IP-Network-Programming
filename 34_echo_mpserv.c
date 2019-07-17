#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// 原书中缓冲设置只有30，长信息会被分割成多条
#define BUF_SIZE 1024
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    char message[BUF_SIZE];
    int str_len, state;
    struct sockaddr_in serv_addr, clnt_addr;
    struct sigaction act;
    socklen_t addr_size=sizeof(clnt_addr);
    pid_t pid;

    if(argc!=2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    act.sa_handler=read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    state=sigaction(SIGCHLD, &act, 0);

    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error!");
    if(listen(serv_sock, 5)==-1)
        error_handling("listen() error!");

    while(1)
    {
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_size);
        if(clnt_sock==-1)
        {
            //error_handling("accept() error!");
            continue;
        }
        else
            puts("new client connected...");
        
        pid=fork();
        if(pid==-1)
        {
            close(clnt_sock);
            continue;
        }
        if(pid==0)
        {
            // 如果是子进程，进行如下操作
            // 子进程中不需要用到服务器套接字，由于也传递了进来因此将其关闭
            close(serv_sock);
            while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
                write(clnt_sock, message, str_len);
            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        }
        else
            // 如果是父进程，进行如下操作
            // 客户端套接字已经传递给子进程，父进程中的客户端套接字可关闭
            close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid=waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
