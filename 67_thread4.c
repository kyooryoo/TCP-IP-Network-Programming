#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100
void* thread_inc(void *arg);
void* thread_des(void *arg);
long long num=0;

int main(int argc, char *argv[])
{
    pthread_t thread_id[NUM_THREAD];
    int i;

    printf("sizeof long long: %lu \n", sizeof(long long));
    for(i=0; i<NUM_THREAD; i++) {
        if(i%2)
            pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
        else
            pthread_create(&(thread_id[i]), NULL, thread_des, NULL);

        // 反注释以下语句，注释掉后面统一添加线程的语句，也可以解决临界区问题
        // 在创建集成后立即将其加入流程，这样是比较合理的操作
        // pthread_join(thread_id[i], NULL);
    }

    // 在统一创建进程后统一添加进程也是造成临界区问题的原因
    // 这里统一创建和加入大量进程，而不是逐一顺次操作，就是为了演示临界区问题
    for(i=0; i<NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);

    printf("result: %lld \n", num);
    return 0;
}

void* thread_inc(void *arg) {
    int i;
    for(i=0; i<50000000; i++)
        num+=1;
    return NULL;
}

void* thread_des(void *arg) {
    int i;
    for(i=0; i<50000000; i++)
        num-=1;
    return NULL;
}