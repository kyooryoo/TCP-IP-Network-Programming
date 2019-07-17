#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int status;

    // 创建第一个子进程并返回3
    pid_t pid=fork();
    if(pid==0)
        return 3;
    else
    {
        // 打印出第一个子进程的PID
        printf("child PID: %d\n", pid);
        // 创建第二个子进程并返回7
        pid=fork();
        if(pid==0)
            exit(7);
        else
        {
            // 打印出第二个子进程的PID
            printf("child PID: %d\n", pid);

            // 第一个终止的子进程信息保存到`status`变量
            wait(&status);
            if(WIFEXITED(status))
                printf("child exit one: %d\n", WEXITSTATUS(status));
            // 第二个终止的子进程信息保存到`status`变量
            wait(&status);
            if(WIFEXITED(status))
                printf("child exit one: %d\n", WEXITSTATUS(status));
            
            // 暂停父进程，此时可以用`ps au`查看子进程状态
            sleep(30);
        }
    }
    return 0;
}