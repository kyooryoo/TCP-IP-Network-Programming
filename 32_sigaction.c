#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// 定义警告信号处理函数
void timeout(int sig)
{
    if(sig==SIGALRM)
        printf("Time out!\n");
    // 通过已注册的警告信号处理呼叫本函数
    // 每隔2秒重复产生SIGALARM信号
    alarm(2);
}

int main(int argc, char *argv[])
{
    int i;
    // 创建sigaction结构体并初始化
    struct sigaction act;
    act.sa_handler=timeout;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    // 调用sigaction函数
    sigaction(SIGALRM, &act, 0);
    // 触发第一个2秒后的警告信号
    alarm(2);

    for(i=0; i<3; i++)
    {
        printf("wait...\n");
        sleep(10);
    }
    return 0;
}