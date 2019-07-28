#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100
void* thread_inc(void *arg);
void* thread_des(void *arg);
long long num=0;
// 声明一个互斥量系统锁
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
    pthread_t thread_id[NUM_THREAD];
    int i;
    // 初始化互斥量
    pthread_mutex_init(&mutex, NULL);

    printf("sizeof long long: %lu \n", sizeof(long long));
    for(i=0; i<NUM_THREAD; i++) {
        if(i%2)
            pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
        else
            pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
    }

    for(i=0; i<NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);

    printf("result: %lld \n", num);
    return 0;
}

void* thread_inc(void *arg) {
    int i;
    // 互斥量调用在循环外部的情况
    // 进入临界区操作前锁定互斥量
    // pthread_mutex_lock(&mutex);
    for(i=0; i<5000000; i++) {
        // 为配合程序代号70测试循环内互斥量的效率
        pthread_mutex_lock(&mutex);
        num+=1;
        // 为配合程序代号70测试循环内互斥量的效率
        pthread_mutex_unlock(&mutex);
    }
    // 退出临界区操作后解锁互斥量
    // pthread_mutex_unlock(&mutex);
    return NULL;
}

void* thread_des(void *arg) {
    int i;
    // 为了对比与程序代号70的运行效率
    // pthread_mutex_lock(&mutex);
    for(i=0; i<5000000; i++) {
        // 互斥量调用在循环内部的情况
        // 进入临界区操作前锁定互斥量
        pthread_mutex_lock(&mutex);
        num-=1;
        // 退出临界区操作后解锁互斥量
        pthread_mutex_unlock(&mutex);
    }
    // 为了对比与程序代号70的运行效率
    // pthread_mutex_unlock(&mutex);
    return NULL;
}