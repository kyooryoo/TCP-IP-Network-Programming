#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int status;

    // 创建一个子进程
    pid_t pid=fork();
    // 延迟子进程的执行15秒
    if(pid==0)
    {
        sleep(15);
        return 24;
    }
    else
    {
        // 每隔3秒调用`waitpid`函数尝试销毁子进程
        while(!waitpid(-1, &status, WNOHANG))
        //while(!wait(&status))
        {
            sleep(3);
            printf("waitpid sleeps 3sec.\n");
        }

        if(WIFEXITED(status))
            printf("child exit: %d\n", WEXITSTATUS(status));
    }
    return 0;
}