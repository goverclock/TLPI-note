/* 编写一个程序使用 getrusage() RUSAGE_CHILDREN 标记获取 wait()调用所等待的
子进程相关的信息。（让程序创建一个子进程并使子进程消耗一些 CPU 时间，接着
让父进程在调用 wait()前后都调用 getrusage()。） */

/* 
getrusage(int who, struct rusage *res_usage)
指定who为RUSAGE_CHILDREN时,将返回调用进程的所有被终止和处于等待状态的子进程相关的信息.
代码直接复制了答案= =

注意书上提到过,子进程的资源使用值只有在其父进程使用wait()之后才会记录到父进程的RUSAGE_CHILDREN值中.

$ ./a.out 
Child terminating
Before wait: user CPU=0.00 secs; system CPU=0.00 secs
After wait:  user CPU=0.68 secs; system CPU=2.32 secs
 */

#include "tlpi_hdr.h"
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <sys/resource.h>

#define NSECS 3 /* Amount of CPU to consume in child */

#define SIG                                          \
    SIGUSR1 /* Child uses this signal to tell parent \
               that it is about to terminate */

static void handler(int sig) { /* Do nothing: just interrupt sigsuspend() */ }

static void printChildRusage(const char *msg) {
    struct rusage ru;

    printf("%s", msg);
    if (getrusage(RUSAGE_CHILDREN, &ru) == -1) errExit("getrusage");
    printf("user CPU=%.2f secs; system CPU=%.2f secs\n",
           ru.ru_utime.tv_sec + ru.ru_utime.tv_usec / 1000000.0,
           ru.ru_stime.tv_sec + ru.ru_stime.tv_usec / 1000000.0);
}

int main(int argc, char *argv[]) {
    clock_t start;
    sigset_t mask;
    struct sigaction sa;

    setbuf(stdout, NULL); /* Disable buffering of stdout */

    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) == -1) errExit("sigaction");

    /* Child informs parent of impending termination using a signal;
       block that signal until the parent is ready to catch it. */

    sigemptyset(&mask);
    sigaddset(&mask, SIG);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) errExit("sigprocmask");

    switch (fork()) {
        case -1:
            errExit("fork");

        case 0: /* Child */
            for (start = clock(); clock() - start < NSECS * CLOCKS_PER_SEC;)
                continue; /* Burn NSECS seconds of CPU time */
            printf("Child terminating\n");

            /* Tell parent we're nearly done */

            if (kill(getppid(), SIG) == -1) errExit("kill");
            _exit(EXIT_SUCCESS);

        default: /* Parent */
            sigemptyset(&mask);
            sigsuspend(&mask); /* Wait for signal from child */

            sleep(2); /* Allow child a bit more time to terminate */

            printChildRusage("Before wait: ");
            if (wait(NULL) == -1) errExit("wait");
            printChildRusage("After wait:  ");
            exit(EXIT_SUCCESS);
    }
}
