/* 尽管 Linux 将 alarm()实现为系统调用，但这当属蛇足。请用 setitimer()实现 alarm()。 */

/* 
alarm()采用实时定时(ITIMER_REAL),一次性.
当其参数为0时,取消计时,这与setitimer()刚好一致.
alarm()到期时,向调用进程发送SIGALRM信号.
而setitimer()到期时,向调用进程发送的信号取决于第一个参数,当为ITIMER_REAL时
刚好就是SIGALRM.
 */

#include "tlpi_hdr.h"
#include <sys/time.h>

unsigned int my_alarm(unsigned int seconds) {
    struct itimerval itv, old;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = seconds;
    itv.it_value.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &itv, &old) == -1) errExit("setitimer");
    return old.it_value.tv_sec;
}

int main() {



    exit(EXIT_SUCCESS);
}
