#include <stdio.h>
#include <unistd.h>

// 创建一个全局变量
int gval=10;
int main(int argc, char *argv[])
{
    // 定义一个进程ID类型的变量
    pid_t pid;
    // 创建一个局部变量
    int lval=20;
    printf("Original Val: [%d %d]\n", gval, lval);
    // 在程序内对全局和局部变量做更改
    gval++, lval+=5;

    printf("Before Process: [%d %d]\n", gval, lval);

    // 保存fork函数返回值，子进程返回0，父进程返回子进程ID
    pid=fork();
    // 如果是子进程在运行
    if(pid==0)
        gval+=2, lval+=2;
    // 如果是父进程在运行
    else
        gval-=2, lval-=2;

    if(pid==0)
        printf("Child Proc: [%d %d]\n", gval, lval);
    else
        printf("Parent Proc: [%d %d]\n", gval, lval);
    return 0;
}