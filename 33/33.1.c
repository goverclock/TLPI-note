/* 编写程序以便证明：作为函数 sigpending()的返回值，同一个进程中的的不同线程
可以拥有不同的 pending 信号。可以使用函数 pthread_kill()分别发送不同的信号给
阻塞这些信号的两个不同的线程，接着调用 sigpending()方法并显示这些 pending 信
号的信息。（可能会发现程序清单 20-4 中函数的作用。） */

/* 
记住先在主线程中阻塞所有信号,再创建其他线程.

[VM-4-12-ubuntu:33]$ ./33.1 
thread 1:
                18 (Continued)
thread 2:
                17 (Child exited)
 */

#include "tlpi_hdr.h"
#include <pthread.h>
#include <signal.h>

// code from signal_functions.c

void                    /* Print list of signals within a signal set */
printSigset(FILE *of, const char *prefix, const sigset_t *sigset)
{
    int sig, cnt;

    cnt = 0;
    for (sig = 1; sig < NSIG; sig++) {
        if (sigismember(sigset, sig)) {
            cnt++;
            fprintf(of, "%s%d (%s)\n", prefix, sig, strsignal(sig));
        }
    }

    if (cnt == 0)
        fprintf(of, "%s<empty signal set>\n", prefix);
}

int                     /* Print signals currently pending for this process */
printPendingSigs(FILE *of, const char *msg)
{
    sigset_t pendingSigs;

    if (msg != NULL)
        fprintf(of, "%s", msg);

    if (sigpending(&pendingSigs) == -1)
        return -1;

    printSigset(of, "\t\t", &pendingSigs);

    return 0;
}

void *thread_func(void *arg) {
    int num = (long)arg;
    sleep(num);   // wait for main thread to send signals
    printf("thread %d:\n", num);
    printPendingSigs(stdout, "");
    return NULL;
}

int main() {
    // block all signals
    sigset_t mask_all;
    sigfillset(&mask_all);
    sigprocmask(SIG_SETMASK, &mask_all, NULL);
    
    pthread_t t1, t2;
    int s = pthread_create(&t1, NULL, thread_func, (void*)1);  // thread 1
    if (s != 0) errExitEN(s, "pthread_create");
    s = pthread_create(&t2, NULL, thread_func, (void*)2);      // thread 2
    if (s != 0) errExitEN(s, "pthread_create");

    pthread_kill(t1, SIGCONT);
    pthread_kill(t2, SIGCHLD);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    exit(EXIT_SUCCESS);
}
