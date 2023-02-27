/* 如果实时信号和标准信号在同时等待一个进程，那么 SUSv3 对信号的传递顺序未
予定义。编写一程序来展示 Linux 是如何处理这一情况的。（令程序为所有信号设
置处理器函数，阻塞这些信号并持续一段时间，以便于向其发送各种信号，最后解
除对所有信号的阻塞。） */

/*
运行此程序,并使用send_all来传递一大堆信号给这个进程:
ubuntu@VM-4-12-ubuntu:~/TLPI-note/22$ ./22.2
pid is 4015776                          # 在另一个shell中运行./send_all 4015776
set handler for all signals
now block all signals except SIGQUIT
^\got SIGQUIT continue in 3s
now unblock all signals
caught 4 - Illegal instruction
caught 5 - Trace/breakpoint trap
caught 7 - Bus error
caught 8 - Floating point exception
caught 11 - Segmentation fault
caught 31 - Bad system call
caught 1 - Hangup
caught 2 - Interrupt
caught 6 - Aborted
caught 10 - User defined signal 1
caught 12 - User defined signal 2
caught 13 - Broken pipe
caught 14 - Alarm clock
caught 15 - Terminated
caught 16 - Stack fault
caught 17 - Child exited
caught 20 - Stopped
caught 21 - Stopped (tty input)
caught 22 - Stopped (tty output)
caught 23 - Urgent I/O condition
caught 24 - CPU time limit exceeded
caught 25 - File size limit exceeded
caught 26 - Virtual timer expired
caught 27 - Profiling timer expired
caught 28 - Window changed
caught 29 - I/O possible
caught 30 - Power failure
caught 34 - Real-time signal 0
caught 35 - Real-time signal 1
caught 36 - Real-time signal 2
caught 37 - Real-time signal 3
caught 38 - Real-time signal 4
caught 39 - Real-time signal 5
caught 40 - Real-time signal 6
caught 41 - Real-time signal 7
caught 42 - Real-time signal 8
caught 43 - Real-time signal 9
caught 44 - Real-time signal 10
caught 45 - Real-time signal 11
caught 46 - Real-time signal 12
caught 47 - Real-time signal 13
caught 48 - Real-time signal 14
caught 49 - Real-time signal 15
caught 50 - Real-time signal 16
caught 51 - Real-time signal 17
caught 52 - Real-time signal 18
caught 53 - Real-time signal 19
caught 54 - Real-time signal 20
caught 55 - Real-time signal 21
caught 56 - Real-time signal 22
caught 57 - Real-time signal 23
caught 58 - Real-time signal 24
caught 59 - Real-time signal 25
caught 60 - Real-time signal 26
caught 61 - Real-time signal 27
caught 62 - Real-time signal 28
caught 63 - Real-time signal 29
caught 64 - Real-time signal 30

结论是先(看起来无序地)传递标准信号,再从小到大地传递实时信号.
我一开始搞出来一个反着的结论,因为设置信号处理器的时候没有屏蔽所有信号,导致当进入一个信号处理器时,
还没有输出信息就立刻被其它信号的处理器中断了,所以出现了倒序的输出.
 */

#include "tlpi_hdr.h"
#include <signal.h>

void handler(int sig) {
    // unsafe
    printf("caught %d - %s\n", sig, strsignal(sig));
}

int main() {
    printf("pid is %d\n", getpid());

    puts("set handler for all signals");
    struct sigaction act;
    // *NOTE* block all signals, so that the printf is in the same order as that
    // signals arrived
    if (sigfillset(&act.sa_mask) == -1) errExit("sigfillset");
    act.sa_flags = 0;
    act.sa_handler = handler;
    for (int i = 1; i < NSIG; i++)
        if (sigaction(i, &act, NULL) == -1 && errno != EINVAL)
            errExit("sigaction");

    puts("now block all signals except SIGQUIT");
    sigset_t all_mask, empty_mask;
    if (sigfillset(&all_mask) == -1 || sigemptyset(&empty_mask) == -1)
        errExit("sigxxset");
    if (sigprocmask(SIG_SETMASK, &all_mask, NULL) == -1)
        errExit("sigprocmask allmask");

    // block until SIGQUIT is pending
    sigset_t quit_mask;
    sigemptyset(&quit_mask);
    if (sigaddset(&quit_mask, SIGQUIT) == -1) errExit("sigaddset");
    sigwaitinfo(&quit_mask, NULL);

    puts("got SIGQUIT continue in 3s");
    sleep(3);

    puts("now unblock all signals");
    if (sigprocmask(SIG_SETMASK, &empty_mask, NULL) == -1)
        errExit("sigprocmask emptymask");

    exit(EXIT_SUCCESS);
}
