/* 编写一个运行于 SCHED_FIFO 调度策略下的程序，然后创建一个子进程。在两个
进程中都执行一个能导致进程最多消耗 3 秒 CPU 时间的函数。（这可以通过使用
一个循环并在循环中不断使用 times()系统调用来确定累积消耗的 CPU 时间来完
成。）每当消耗了 1/4 秒的 CPU 时间之后，函数应该打印出一条显示进程 ID 和迄
今消耗的 CPU 时间的消息。每当消耗了 1 秒的 CPU 时间之后，函数应该调用
sched_yield()来将 CPU 释放给其他进程。（另一种方法是进程使用 sched_setparam()
提升对方的调度策略。）从程序的输出中应该能够看出两个进程交替消耗了 1 秒
的 CPU 时间。（注意在 35.3.2 节中给出的有关防止失控实时进程占住 CPU 的建
议。） */

/* 
神奇的是,虽然clock()和times()都返回clock_t,但它们的度量单位是不一样的.
对于clock(),使用CLOCKS_PER_SEC转化为秒,对于times(),使用sysconf(_SC_CLK_TCK)转化为秒.
详见https://linux.die.net/man/2/times

注意,想要看到交替运行的结果,需要先把两个进程都绑定到一个CPU上.

$ gcc ./35.3.c -ltlpi; sudo ./a.out 
4702: 0.250000 seconds
4702: 0.500000 seconds
4702: 0.750000 seconds
4702: 1.000000 seconds
4702: yield
4703: 0.250000 seconds
4703: 0.500000 seconds
4703: 0.750000 seconds
4703: 1.000000 seconds
4703: yield
4702: 2.000000 seconds
4702: 2.000000 seconds
4702: 2.000000 seconds
4702: 2.000000 seconds
4702: yield
4703: 1.250000 seconds
4703: 1.500000 seconds
4703: 1.750000 seconds
4703: 2.000000 seconds
4703: yield
4702: 3.000000 seconds
4702: 3.000000 seconds
4702: 3.000000 seconds
4702: 3.000000 seconds
4702: finish
4703: 2.250000 seconds
4703: 2.500000 seconds
4703: 2.750000 seconds
4703: 3.000000 seconds
4703: finish
$

 */

#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include <sched.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <time.h>

void consume3s(pid_t pid) {
    clock_t begin = times(NULL);
    int i = 1;
    while (1) {
        clock_t cur = times(NULL);
        double seconds = (double)(cur - begin) / sysconf(_SC_CLK_TCK);
        if (seconds >= 0.25 * i) {
            i++;
            printf("%d: %lf seconds\n", pid, seconds);
        } else {
            continue;
        }
        if (i == 13) {
            printf("%d: finish\n", pid);
            return;
        }
        if (!((i - 1) % 4)) {
            printf("%d: yield\n", pid);
            sched_yield();
        }
    }
}

int main() {
    struct sched_param sp;
    sp.sched_priority = 1;
    if (sched_setscheduler(0, SCHED_FIFO, &sp) == -1) {
        errExit("sched_setscheduler");
    }

    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);

    pid_t pid = fork();
    sched_setaffinity(0, sizeof(set), &set);
    consume3s(getpid());
    wait(NULL);

    exit(EXIT_SUCCESS);
}
