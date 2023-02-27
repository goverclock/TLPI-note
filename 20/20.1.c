/* 如 20.3 节所指，比之
signal()，sigaction()函数在建立信号处理器时可移植性更佳。请 用
sigaction()替换程序清单 20-7 程序（sig_receiver.c）中对 signal()的调用。 */

/*
注意需要手动将act.sa_flags和act.sa_mask初始化,方法分别是直接赋值和使用sigemptyset().

设置信号处理器的时候是从1遍历到NSIG-1的,这些数值中并非所有值都定义了信号,
不过意外地发现NSIG的值达到了64左右,并且对超出31的值调用signal不会出错.(update:因为编号32~64是实时信号,见原书22.8节)

信号处理器返回前会自动屏蔽其处理的信号,不需要手动将此信号加入到sa_mask.
那么怎样实现信号处理器不屏蔽自己处理的信号呢?在sa_mask中加入SA_NODEFER即可.

sig_receiver.c第53行有一个语句:
        (void) signal(n, handler);
这是一个函数调用,但为什么要在开头加一个(void)呢?
有很多种情况,在这种情况下我觉得应该只是用来说明有意无视了返回值.

It means very little. It is explicitly turning the line to a void expression,
that is an expression that is only used for its side effects and whose value is
discarded. So the lines func1(); and (void)func1(); will do the same thing.
There could be some optimization that can be performed if the compiler knows the
value of the expression is not used, but it is likely that compiler can figure
it out with or without the explicit (void).

It could be used as a form of documentation where the programmer is trying to
make it obvious that they are not using the value of the expression.

https://stackoverflow.com/questions/13954517/use-of-void-before-a-function-call
 
那么为什么可以忽略signal()的返回值呢,因为其唯一可能出现的错误就是signum为无效值(参见man),在这个程序中
不需要额外处理.
 */

/* sig_receiver.c

   Usage: sig_receiver [block-time]

   Catch and report statistics on signals sent by sig_sender.c.

   Note that although we use signal() to establish the signal handler in this
   program, the use of sigaction() is always preferable for this task.
*/

#include "tlpi_hdr.h"
#include <signal.h>

static int sigCnt[NSIG]; /* Counts deliveries of each signal */
static volatile sig_atomic_t gotSigint = 0;
/* Set nonzero if SIGINT is delivered */

static void handler(int sig) {
    if (sig == SIGINT)
        gotSigint = 1;
    else
        sigCnt[sig]++;
}

int main(int argc, char *argv[]) {
    int n, numSecs;
    sigset_t pendingMask, blockingMask, emptyMask;

    printf("%s: PID is %ld\n", argv[0], (long)getpid());

    /* Here we use the simpler signal() API to establish a signal handler,
       but for the reasons described in Section 22.7 of TLPI, sigaction()
       is the (strongly) preferred API for this task. */

    for (n = 1; n < NSIG; n++) /* Same handler for all signals */
    {
        struct sigaction act;
        act.sa_handler = handler;
        sigemptyset(&act.sa_mask);
        if (sigaddset(&act.sa_mask, n) == -1 && errno != EINVAL) errExit("sigaddset");
        act.sa_flags = 0;

        sigaction(n, &act, NULL);
        // signal(n, handler); /* Ignore errors */
    }

    /* If a sleep time was specified, temporarily block all signals,
       sleep (while another process sends us signals), and then
       display the mask of pending signals and unblock all signals */

    if (argc > 1) {
        numSecs = getInt(argv[1], GN_GT_0, NULL);

        sigfillset(&blockingMask);
        if (sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1)
            errExit("sigprocmask");

        printf("%s: sleeping for %d seconds\n", argv[0], numSecs);
        sleep(numSecs);

        if (sigpending(&pendingMask) == -1) errExit("sigpending");

        printf("%s: pending signals are: \n", argv[0]);
        // printSigset(stdout, "\t\t", &pendingMask);

        sigemptyset(&emptyMask); /* Unblock all signals */
        if (sigprocmask(SIG_SETMASK, &emptyMask, NULL) == -1)
            errExit("sigprocmask");
    }

    while (!gotSigint) /* Loop until SIGINT caught */
        continue;

    for (n = 1; n < NSIG; n++) /* Display number of signals received */
        if (sigCnt[n] != 0)
            printf("%s: signal %d caught %d time%s\n", argv[0], n, sigCnt[n],
                   (sigCnt[n] == 1) ? "" : "s");

    exit(EXIT_SUCCESS);
}
