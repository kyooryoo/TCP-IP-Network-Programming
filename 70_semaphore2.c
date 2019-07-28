#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100

// 以下为从网络上找到的为兼容MacOS而移植的信号量代码
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif

struct rk_sema {
#ifdef __APPLE__
    dispatch_semaphore_t sem;
#else
    sem_t sem;
#endif
};

static inline void rk_sema_init(struct rk_sema *s, uint32_t value) {
#ifdef __APPLE__
    dispatch_semaphore_t *sem = &s->sem;
    *sem = dispatch_semaphore_create(value);
#else
    sem_init(&s->sem, 0, value);
#endif
}

static inline void rk_sema_wait(struct rk_sema *s) {
#ifdef __APPLE__
    dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
#else
    int r;
    do {
        r = sem_wait(&s->sem);
    } while (r == -1 && errno == EINTR);
#endif
}

static inline void rk_sema_post(struct rk_sema *s) {
#ifdef __APPLE__
    dispatch_semaphore_signal(s->sem);
#else
    sem_post(&s->sem);
#endif
}
// 到此为止，移植代码结束

void* thread_inc(void *arg);
void* thread_des(void *arg);

static struct rk_sema sem_one;
static struct rk_sema sem_two;

static long long num=0;

int main(int argc, char *argv[])
{
    pthread_t thread_id[NUM_THREAD];
    int i;

    rk_sema_init(&sem_one, 0);
    rk_sema_init(&sem_two, 1);

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
    rk_sema_wait(&sem_two);
    for(i=0; i<50000000; i++) {
        // 放到循环内你等不到它执行完
        // rk_sema_wait(&sem_two);
        num+=1;
        // 同上，除非把循环次数减少到5000
        // rk_sema_post(&sem_one);
    }
    rk_sema_post(&sem_one);
    return NULL;
}

void* thread_des(void *arg) {
    int i;
    rk_sema_wait(&sem_one);
    for(i=0; i<50000000; i++) {
        // 放到循环内你等不到它执行完
        // rk_sema_wait(&sem_one);
        num-=1;
        // 同上，除非把循环次数减少到5000
        // rk_sema_post(&sem_two);
    }
    rk_sema_post(&sem_two);
    return NULL;
}