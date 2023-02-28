/* 编写一个程序验证：如果调用 timer_create()时将参数 evp 置为 NULL，那么这就等
同于将 evp 设为指向 sigevent 结构的指针，并将该结构中的 sigev_notify 置为 SIGEV_
SIGNAL，将 sigev_signo 置为 SIGALRM，将 si_value.sival_int 置为定时器 ID。 */

/* 
很奇怪的是,不论是此代码(之前的版本)还是tlpi-dist/timers/p_tmr_null_evp.c,运行结果中的sival_int
都不是计时器id.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/23$ ./23.3
timer_create: tid = 93943737066144
caught 14 - Alarm clock
handler: si_value.sival_int = -82599264

ubuntu@VM-4-12-ubuntu:~/TLPI-note/tlpi-dist/timers$ ./ptmr_null_evp 1
timer ID = 94403923612320
[08:12:10] Got signal 14
    sival_int          = 542446240
    si_overrun         = 0
    timer_getoverrun() = 0
^C

最后注意到sival_int是一个int类型,而timer_t为void*,使用long才能输出其完整的值.
所以sival_int中是一个被截断了的值...
下面是当前版本,也就是把两处都视为int处理的版本,输出就一致了:

ubuntu@VM-4-12-ubuntu:~/TLPI-note/23$ g++ ./23.3.c -o 23.3 -lrt -ltlpi -fpermissive; ./23.3
./23.3.c: In function ‘int main()’:
./23.3.c:44:45: warning: cast from ‘timer_t’ {aka ‘void*’} to ‘int’ loses precision [-fpermissive]
   44 |     printf("timer_create: tid = %d\n", (int)tid);
      |                                             ^~~
timer_create: tid = -1376136544
caught 14 - Alarm clock
handler: si_value.sival_int = -1376136544

 */

#include "tlpi_hdr.h"
#include <signal.h>
#include <time.h>
#include <sys/time.h>

void handler(int sig, siginfo_t *si, void *uc) {
    printf("caught %d - %s\n", sig, strsignal(sig));
    int tid = si->si_value.sival_int;
    printf("handler: si_value.sival_int = %d\n", tid);
}

int main() {
    // set up handler
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGALRM, &act, NULL) == -1) errExit("sigaction");

    timer_t tid;
    if (timer_create(CLOCK_REALTIME, NULL, &tid) == -1) errExit("timer_create");
    printf("timer_create: tid = %d\n", (int)tid);
    struct itimerspec its;
    its.it_value.tv_sec = 3;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 3;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(tid, 0, &its, NULL) == -1) errExit("timer_settime");

    pause();

    return 0;
}
