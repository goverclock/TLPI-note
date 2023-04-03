/* 修改程序清单 30-1（thread_incr.c）中的程序，以便线程起始函数在每次循环中都
能输出 glob 的当前值以及能对线程做唯一标识的标识符。可将线程的这一唯一标
识指定为创建线程的函数 pthread_create()的调用参数。对于这一程序，需要将线程
起始函数的参数改为指针，指向包含线程唯一标识和循环次数限制的数据结构。运
行该程序，将输出重定向至一文件，查看内核在调度两线程交替执行时 glob 的变
化情况。 */

/* 
为原来的程序添加了正确的互斥锁.两个线程分别用a和b标识.
输出结果在res文件中.可以看出来一半会是一个线程循环了很多次之后才会被调度出去.
不会出现类似于aba这种情况.
 */

#include <pthread.h>
#include "tlpi_hdr.h"

struct info {
    char id;
    int loop;
};

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static volatile int glob = 0;   /* "volatile" prevents compiler optimizations
                                   of arithmetic operations on 'glob' */
static void *                   /* Loop 'arg' times incrementing 'glob' */
threadFunc(void *arg)
{
    int loops = ((struct info*)arg)->loop;
    char id = ((struct info*)arg)->id; 
    int loc, j;

    for (j = 0; j < loops; j++) {
        pthread_mutex_lock(&mtx);
        glob++;
        printf("id=%c glob=%d\n", id, glob);
        pthread_mutex_unlock(&mtx);
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
    struct info info1, info2;
    int  s;

    info1.loop = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 1000;
    info2.loop = info1.loop;

    info1.id = 'a';     // who needs tid lol
    info2.id = 'b';
    s = pthread_create(&t1, NULL, threadFunc, &info1);
    if (s != 0)
        errExitEN(s, "pthread_create");
    s = pthread_create(&t2, NULL, threadFunc, &info2);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}
