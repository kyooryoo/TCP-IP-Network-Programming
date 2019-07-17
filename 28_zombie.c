#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid=fork();

    // 子进程会在父进程后运行，进入这里
    if(pid==0)
        printf("child process working!\n");
    else
    // 父进程会先运行，进入这里，然后等待30秒
    {
        printf("parent process working!\n");
        printf("child process ID: %d\n", pid);
        sleep(30);
    }

    // 在父进程等待期间，子进程先结束
    if(pid==0)
        puts("End of child process!");
    // 父进程在等待时间超时后也相应的结束
    else
        puts("End of parent process!");
    
    return 0;
}