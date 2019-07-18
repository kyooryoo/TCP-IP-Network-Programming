#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int fds[2];
    char str1[]="How are you?";
    char str2[]="I'm good. How about you?";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds);
    pid=fork();
    if(pid==0)
    {
        write(fds[1], str1, sizeof(str1));
        // 实际上注释掉下面一行等待命令也不会引发错误
        sleep(2);
        read(fds[0], buf, BUF_SIZE);
        printf("Child proc output: %s \n", buf);
    }
    else
    {
        read(fds[0], buf, BUF_SIZE);
        printf("Parent proc output: %s \n", buf);
        write(fds[1], str2, sizeof(str2));
        // 下面一行等待命令也可以注释掉，不会引发错误
        sleep(3);
    }
    return 0;
}