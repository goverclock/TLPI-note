/* 若一线程执行了如下代码，可能会产生什么结果？
    pthread_join(pthread_self(), NULL);
 在 Linux 上编写一个程序，观察一下实际会发生什么情况。假设代码中有一变量 tid，
其中包含了某个线程 ID，在自身发起 pthread_join(tid, NULL)调用时，要避免造成
与上述语句相同的后果，该线程应采取何种措施？ */

/* 
运行的结果:
ubuntu@VM-4-12-ubuntu:~/TLPI-note/29$ ./29.1 
ERROR [EDEADLK/EDEADLOCK Resource deadlock avoided] pthread_join in thread_func
效果就是pthread_join(pthread_self(), NULL)这条语句会立刻返回,返回值为EDEADLK.

要避免这种情况很简单,只需要事先检查一下目标线程ID是否就是自己的线程ID:
if (!pthread_equal(tid, pthread_self()))
    pthread_join(tid, NULL);
 */

#include "tlpi_hdr.h"
#include <errno.h>
#include <pthread.h>

void *thread_func(void *arg) {
    int s = pthread_join(pthread_self(), NULL);
    if (s != 0) errExitEN(s, "pthread_join in thread_func");
    pthread_exit(NULL);
}

int main() {
    pthread_t tid;
    void *res;

    int s = pthread_create(&tid, NULL, thread_func, NULL);
    if (s != 0) errExitEN(s, "pthread_create");

    pthread_join(tid, &res);

    exit(EXIT_SUCCESS);
}
