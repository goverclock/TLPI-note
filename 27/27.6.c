/* 假设父进程为信号 SIGCHLD 创建了一处理器程序，同时阻塞该信号。随后，其某
一子进程退出，父进程接着执行 wait()以获取该子进程的状态。当父进程解除对
SIGCHLD 的阻塞时，会发生什么？编写一个程序来验证答案。这一结果与调用
system()函数的程序之间有什么关联？ */

/* 
注意:wait()调用和SIGCHLD是相互独立的.
结果当然是父进程的wait()调用返回,然后解除阻塞时SIGCHLD信号会触发其信号处理器.
但是这时候子进程已经被回收了,如果再在SIGCHLD的信号处理器中调用wait()会因为没有
子进程而出错.(假设没有其它子进程)

system()运行期间是会阻塞SIGCHLD的.否则:
(书中原文)当由 system()所创建的子进程退出并产生 SIGCHLD 信号时，在 system()有机会调用
waitpid()之前，主程序的信号处理器程序可能会率先得以执行

所以当system()返回并解除对SIGCHLD的阻塞时,也会发生上述情况.

下面的程序同时演示了两种情况.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/27$ ./27.6
lol caught 17 - Child exited
wtf wait() failed: No child processes
lol caught 17 - Child exited
wtf wait() failed: No child processes
 */

#include "tlpi_hdr.h"
#include <signal.h>
#include <sys/wait.h>

void handler(int sig) {
    // unsafe
    printf("lol caught %d - %s\n", sig, strsignal(sig));
    if (wait(NULL) == -1) {
        printf("wtf wait() failed: %s\n", strerror(errno));
    }
}

int main() {
    // block SIGCHLD
    sigset_t block_mask, orig_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &block_mask, &orig_mask);

    // handle SIGCHLD
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    // do something
    pid_t child_pid;
    switch (child_pid = fork()) {
        case -1:
            errExit("fork");
        case 0:     // child
            sleep(1);
        default:    // parent
            waitpid(child_pid, NULL, 0);
    }

    // or
    system("sleep 1");

    // unblock SIGCHLD
    sigprocmask(SIG_SETMASK, &orig_mask, NULL);

    exit(EXIT_SUCCESS);
}
