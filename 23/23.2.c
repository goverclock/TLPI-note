/* 试着将程序清单 23-3（t_nanosleep.c）程序置于后台运行，并设置 60 秒的休眠间隔，
同时使用如下命令发送尽可能多的 SIGINT 信号给后台进程：
$ while true; do kill -INT pid; done
应该能注意到程序休眠时间要长于预期。将 nanosleep()用 clock_gettime()（使用
CLOCK_REALTIME 时钟）和设置 TIMER_ABSTIME 标志的 clock_nanosleep()来替
换。（此练习需要 Linux 2.6 版本。）反复测试修改后的程序，并解释新老程序间的
差别。 */

/* 
书上23.4.2节介绍了在高频接收信号的情况下出现的"嗜睡"问题.
这里的是该用clock_xxx()调用的新程序,注意到计算出的已休眠时间和剩余休眠时间只和与理论值
稍有差异,这很容易理解,因为clock_nanosleep()和clock_gettime()(获取当前时间)之间进程
可能会被内核调度出去.或者说这两条语句之间本来就是有时间差的.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/23$ ./t 10 0
^CSlept for:  1.366296 secs
Remaining:  8.633616856
^CSlept for:  1.856976 secs
Remaining:  8.143010072
...
 */

#define _POSIX_C_SOURCE 199309
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void sigintHandler(int sig) { return; /* Just interrupt nanosleep() */ }

int main(int argc, char *argv[]) {
    struct timeval start, finish;
    struct timespec request, remain;
    struct sigaction sa;
    int s;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s secs nanosecs\n", argv[0]);

    request.tv_sec = getLong(argv[1], 0, "secs");
    request.tv_nsec = getLong(argv[2], 0, "nanosecs");

    /* Allow SIGINT handler to interrupt nanosleep() */

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigintHandler;
    if (sigaction(SIGINT, &sa, NULL) == -1) errExit("sigaction");

    if (gettimeofday(&start, NULL) == -1) errExit("gettimeofday");

    struct timespec new_req;
    if (clock_gettime(CLOCK_REALTIME, &new_req) == -1)
        errExit("clock_gettime new_req");
    new_req.tv_sec += request.tv_sec;
    for (;;) {
        s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &new_req, NULL);
        if (s == -1 && errno != EINTR) errExit("nanosleep");

        if (gettimeofday(&finish, NULL) == -1) errExit("gettimeofday");
        printf("Slept for: %9.6f secs\n",
               finish.tv_sec - start.tv_sec +
                   (finish.tv_usec - start.tv_usec) / 1000000.0);

        if (s == 0) break; /* nanosleep() completed */
        struct timespec cur_time;
        if (clock_gettime(CLOCK_REALTIME, &cur_time) == -1)
            errExit("clock_gettime cur_time");
        remain.tv_sec = new_req.tv_sec - cur_time.tv_sec;
        remain.tv_nsec = new_req.tv_nsec - cur_time.tv_nsec;
        if (remain.tv_nsec < 0) {
            remain.tv_sec--;
            remain.tv_nsec += 1000000000;
        }
        printf("Remaining: %2ld.%09ld\n", (long)remain.tv_sec, remain.tv_nsec);
    }

    printf("Sleep complete\n");
    exit(EXIT_SUCCESS);
}
