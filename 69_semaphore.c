#include <stdio.h>
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

void* input(void *arg);
void* accu(void *arg);

// 声明两个信号量
static struct rk_sema sem_one;
static struct rk_sema sem_two;

static int num;

int main(int argc, char *argv[])
{
    pthread_t id_t1, id_t2;
    // 初始化信号量值分别为0和1，即一个为关另一个为开
    rk_sema_init(&sem_one, 0);
    rk_sema_init(&sem_two, 1);

    pthread_create(&id_t1, NULL, input, NULL);
    pthread_create(&id_t2, NULL, accu, NULL);
    pthread_join(id_t1, NULL);
    pthread_join(id_t2, NULL);

    // MacOS的Dispatch Semaphore没有提供销毁功能的函数
    // sem_destroy(&sem_one);
    // sem_destroy(&sem_two);
    return 0;
}

void* input(void *arg) {
    int i;
    for(i=0; i<5; i++) {
        fputs("Input num: ", stdout);
        // 检查并关闭第二个信号， 读取输入，开启第一个信号
        rk_sema_wait(&sem_two);
        scanf("%d", &num);
        rk_sema_post(&sem_one);
    }
    return NULL;
}

void* accu(void *arg) {
    int sum=0, i;
    for(i=0; i<5; i++) {
        // 检查关闭第一个信号，做累加操作，开启第二个信号
        rk_sema_wait(&sem_one);
        sum+=num;
        rk_sema_post(&sem_two);
    }
    printf("Result: %d \n", sum);
    return NULL;
}