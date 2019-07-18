#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int fds[2];
    char str[]="How are you?";
    char buf[BUF_SIZE];
    pid_t pid;

    // 创建管道，保存I/O的文件描述符到数组变量
    pipe(fds);
    // 复制子进程，父子进程都有了I/O文件描述符
    pid=fork();
    if(pid==0)
        // 传递管道入口给子进程
        write(fds[1], str, sizeof(str));
    else
    {
        // 传递管道出口给父进程，并输出结果
        read(fds[0], buf, BUF_SIZE);
        puts(buf);
    }
    return 0;
}