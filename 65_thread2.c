#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char *argv[])
{
    pthread_t t_id;
    int thread_param=5;
    // 定义一个指针用于保存线程主程序返回值
    void *thr_ret;

    if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0) {
        puts("pthread_create() error!");
        return -1; };
    // 将当前进程加入指定线程的等待队列，等待指定线程执行完后再继续
    if(pthread_join(t_id, &thr_ret)!=0) {
        puts("pthread_join() error!");
        return -1; }
    // 验证指针中已经保存了之前结束线程所返回的字符串
    printf("Thread return message: %s \n", (char*)thr_ret);
    // 这里free方法的作用不明了
    free(thr_ret);
    return 0;
}

void* thread_main(void *arg) {
    int i, cnt=*((int*)arg);
    // 定义一个字符串指针，预留50个字节大小
    char *msg=(char *)malloc(sizeof(char)*50);
    // 拷贝指定的字符串内容到之前定义的字符串指针
    strcpy(msg, "Hello, this is a thread!");
    for(i=0; i<cnt; i++) {
        sleep(1);
        puts("running thread"); }
    // 返回之前定义的字符串指针
    return (void*)msg;
}