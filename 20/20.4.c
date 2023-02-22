/* 请用 sigaction()调用来实现 siginterrupt()。
(后者在21.5节介绍) */

/* 
书上对这个函数的介绍:

int siginterrupt(int sig, int flag);
    Returns 0 on success, or -1 on error

若参数 flag 为真（1），则针对信号 sig 的处理器函数将会中断阻塞的系统调用的执行。如
果 flag 为假（0），那么在执行了 sig 的处理器函数之后，会自动重启阻塞的系统调用。
函数 siginterrupt()的工作原理是：调用 sigaction()获取信号当前处置的副本，调整自结构
oldact 中返回的 SA_RESTART 标志，接着再次调用 sigaction()来更新信号处置。
SUSv4 标记 sigterrupt()为已废止，并推荐使用 sigaction()加以替代。
 */

#include "tlpi_hdr.h"
#include <signal.h>

int my_siginterrupt(int sig, int flag) {
    struct sigaction act;
    if (sigaction(sig, NULL, &act) == -1) return -1;
    if (flag)
        act.sa_flags &= ~SA_RESTART;
    else
        act.sa_flags |= SA_RESTART;
    return sigaction(sig, &act, NULL);
}
