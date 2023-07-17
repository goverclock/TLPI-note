/* 修改程序清单 34-4（disc_SIGHUP.c）来验证当控制进程在收到 SIGHUP 信号而不
终止时，内核不会向前台进程组中的成员发送 SIGHUP 信号。 */

/* 
当会话首进程打开了一个控制终端之后它同时也成为了该终端的控制进程。在发生终端
断开之后，内核会向控制进程发送一个 SIGHUP 信号来通知这一事件的发生。

书中34.6.2原文:

如果因为终端断开引起的向控制进程发送的 SIGHUP 信号会导致控制进程终止，那么
SIGHUP 信号会被发送给终端的前台进程组中的所有成员（见 25.2 节）。这个行为是控制进程终
止的结果，而不是专门与 SIGHUP 信号关联的行为。如果控制进程出于任何原因终止，那么前
台进程组就会收到 SIGHUP 信号。

修改前:

# exec会替换掉shell进程,从而成为终端控制进程
$ exec ./disc_SIGHUP d s > sig.log 2>&1     # 创建两个子进程,一个与父进程位于同一进程组,另一个位于单独进程组

然后直接关掉终端,打开一个新的终端查看sig.log:

PID of parent process is:       31825
Foreground process group ID is: 31825
PID=31825 PGID=31825
PID=31993 PGID=31993                    # 非前台进程组成员,后面没有接收到SIGHUP
PID=31994 PGID=31825
PID 31994: caught signal  1 (Hangup)    # 前台进程组中的成员接收到SIGHUP
# 注意,父进程接收到了SIGHUP信号后会直接终止,因为disc_SIGHUP.c没有给父进程设置SIGHUP的信号处置
# 所以产生了SIGHUP的默认行为:终止进程

本文件为修改后的程序,对父进程也设置了SIGHUP的信号处置

$ exec ./a.out d s > sig.log 2>&1

sig.log:

PID of parent process is:       5070
Foreground process group ID is: 5070
PID=5296 PGID=5296
PID=5070 PGID=5070
PID=5297 PGID=5070
PID 5070: caught signal  1 (Hangup)     # 父进程没有终止,所以前台进程组的其他成员没有接收到SIGHUP

 */


#define _GNU_SOURCE     /* Get strsignal() declaration from <string.h> */
#include "tlpi_hdr.h"
#include <string.h>
#include <signal.h>

static void             /* Handler for SIGHUP */
handler(int sig)
{
    printf("PID %ld: caught signal %2d (%s)\n", (long) getpid(),
            sig, strsignal(sig));
                        /* UNSAFE (see Section 21.1.2) */
}

int
main(int argc, char *argv[])
{
    pid_t parentPid, childPid;
    int j;
    struct sigaction sa;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s {d|s}... [ > sig.log 2>&1 ]\n", argv[0]);

    setbuf(stdout, NULL);               /* Make stdout unbuffered */

    parentPid = getpid();
    printf("PID of parent process is:       %ld\n", (long) parentPid);
    printf("Foreground process group ID is: %ld\n",
            (long) tcgetpgrp(STDIN_FILENO));

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(SIGHUP, &sa, NULL) == -1)
        errExit("sigaction");

    for (j = 1; j < argc; j++) {        /* Create child processes */
        childPid = fork();
        if (childPid == -1)
            errExit("fork");

        if (childPid == 0) {            /* If child... */
            if (argv[j][0] == 'd')      /* 'd' --> to different pgrp */
                if (setpgid(0, 0) == -1)
                    errExit("setpgid");
            break;                      /* Child exits loop */
        } 
    }

    /* All processes fall through to here */

    alarm(60);      /* Ensure each process eventually terminates */

    printf("PID=%ld PGID=%ld\n", (long) getpid(), (long) getpgrp());
    for (;;)
        pause();        /* Wait for signals */
}
