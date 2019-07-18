#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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
    // 准备保存通道文件描述符的数组
    int fds[2];

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

    // 创建管道，用于在父子进程之间传递回声信息
    pipe(fds);
    // 这里创建的子进程将用于保存回声信息文件
    pid=fork();
    if(pid==0)
    {
        FILE * fp=fopen("echomsg.txt", "wt");
        char msgbuf[BUF_SIZE];
        int i, len;

        for(i=0; i<10; i++)
        {
            // 从管道出口读取回声信息并保存到文本文件
            len=read(fds[0], msgbuf, BUF_SIZE);
            fwrite((void*)msgbuf, 1, len, fp);
        }
        fclose(fp);
        return 0;
    }

    while(1)
    {
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_size);
        if(clnt_sock==-1)
            continue;
        else
            puts("new client connected...");
        
        pid=fork();
        if(pid==0)
        {
            close(serv_sock);
            while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
            {
                // 在读取到客户端发来的回声信息后写入客户端套接字和管道入口
                write(clnt_sock, message, str_len);
                write(fds[1], message, str_len);
            }
            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        }
        else
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
