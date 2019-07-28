#include <stdio.h>
// sleep函数需要以下unistd.h库
#include <unistd.h>
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char *argv[])
{
    // 声明进程ID和初始化进程主程序的参数
    pthread_t t_id;
    int thread_param=5;
    // 创建进程，使用默认属性，指定单独定义的进程主程序，指定其参数变量地址，
    if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0) {
        puts("pthread_create() error!");
        return -1; };
    // 进程执行结束后等地十秒打印结束信息并返回
    sleep(10);
    puts("end of main!");
    return 0;
}

void* thread_main(void *arg) {
    int i, cnt=*((int*)arg);
    // 在进程执行其间每隔一秒打印一条信息，次数由参数指定
    for(i=0; i<cnt; i++) {
        // 更改等待时间为2，再编译和运行可验证进程先结束的后果
        sleep(1);
        puts("running thread"); }
    return NULL;
}