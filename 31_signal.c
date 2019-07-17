#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
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

void keycontrol(int sig)
{
    if(sig==SIGINT)
        printf("CTRL+C pressed!\n");
}

int main(int argc, char *argv[])
{
    int i;
    // 注册SIGALARM和SIGINT信号处理器
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);
    // 触发第一个2秒后的警告信号
    alarm(2);

    for(i=0; i<3; i++)
    {
        printf("wait...\n");
        sleep(10);
    }
    return 0;
}