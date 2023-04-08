/* 试实现函数 one_time_init(control，init)，要求与函数 pthread_once()执行等同操作。
参数 control 应为一指针，指向经静态分配的结构，其中包含一个布尔型变量和一
个互斥量。布尔型变量用以标识函数 init 是否曾被调用过，而由互斥量来控制对变
量的访问。为简化函数实现，可以忽略诸如 init()调用失败或者在由线程初次调用
时被取消的情况（亦即，无需为此做特别设计，如果真发生了此类事件，那么下一
个调用 one_time_init()的线程会重新调用 init()）。 */

/* 
运行结果:

[VM-4-12-ubuntu:31]$ ./31.1 
Called init_func()
First thread returned
Second thread returned

 */

#include <pthread.h>
#include "tlpi_hdr.h"

struct one_control {
    bool called;
    pthread_mutex_t mtx;
};

static struct one_control once = {false, PTHREAD_MUTEX_INITIALIZER};

int one_time_init(struct one_control *ctl, void(*init_routinue)(void)) {
    int s = pthread_mutex_lock(&ctl->mtx);
    if (s != 0) errExitEN(s, "pthread_mutex_lock");
    if (!ctl->called) {
        init_routinue();
        ctl->called = true;
    }
    s = pthread_mutex_unlock(&ctl->mtx);
    if (s != 0) errExitEN(s, "pthread_mutex_lock");

    return 0;
}

// 下面的代码是从tlpi-dist/threads/one_time_init.c复制过来的测试代码.

/* Remaining code is for testing one_time_init() */

static void
init_func()
{
    /* We should see this message only once, no matter how many
       times one_time_init() is called */

    printf("Called init_func()\n");
}

static void *
threadFunc(void *arg)
{
    /* The following allows us to verify that even if a single thread calls
       one_time_init() multiple times, init_func() is called only once */

    one_time_init(&once, init_func);
    one_time_init(&once, init_func);
    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int s;

    /* Create two threads, both of which will call one_time_init() */

    s = pthread_create(&t1, NULL, threadFunc, (void *) 1);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_create(&t2, NULL, threadFunc, (void *) 2);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    printf("First thread returned\n");

    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    printf("Second thread returned\n");

    exit(EXIT_SUCCESS);
}
