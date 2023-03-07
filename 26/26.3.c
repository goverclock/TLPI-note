/* 使用 waitid()替换程序清单 26-3（child_status.c）中的 waitpid()。需要将对函数
print WaitStatus()的调用替换为打印 waitid()所返回 siginfo_t
结构中相关字段的代码。 */

/* 
不知道为什么没有WCONTINUED的定义.

 */

#include <sys/wait.h>
#include <sys/types.h>
#include "tlpi_hdr.h"

void /* Examine a wait() status using the W* macros */
printWaitStatus(const char *msg, const siginfo_t *si) {
    if (msg != NULL) printf("%s", msg);
    int status = si->si_status;

    if (si->si_code == CLD_EXITED) {
        printf("child exited, status=%d\n", status);
    } else if (si->si_code == CLD_KILLED) {
        printf("child killed by signal %d (%s)", status, strsignal(status));
#ifdef WCOREDUMP /* Not in SUSv3, may be absent on some systems */
        if (WCOREDUMP(status)) printf(" (core dumped)");
#endif
        printf("\n");

    } else if (si->si_code == CLD_STOPPED) {
        printf("child stopped by signal %d (%s)\n", status, strsignal(status));

#ifdef WIFCONTINUED /* SUSv3 has this, but older Linux versions and \
                       some other UNIX implementations don't */
    } else if (WIFCONTINUED(status)) {
        printf("child continued\n");
#endif

    } else { /* Should never happen */
        printf("what happened to this child? (status=%x)\n",
               (unsigned int)status);
    }
}

int main(int argc, char *argv[]) {
    int status;
    pid_t childPid;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [exit-status]\n", argv[0]);

    switch (fork()) {
        case -1:
            errExit("fork");

        case 0: /* Child: either exits immediately with given
                   status or loops waiting for signals */
            printf("Child started with PID = %ld\n", (long)getpid());
            if (argc > 1) /* Status supplied on command line? */
                exit(getInt(argv[1], 0, "exit-status"));
            else /* Otherwise, wait for signals */
                for (;;) pause();
            exit(EXIT_FAILURE); /* Not reached, but good practice */

        default: /* Parent: repeatedly wait on child until it
                    either exits or is terminated by a signal */
            for (;;) {
                siginfo_t si;
                childPid = waitid(P_ALL, 0, &si,
                                  WEXITED | WSTOPPED
#ifdef WCONTINUED
                                      | WCONTINUED
#endif
                );
                if (childPid == -1) errExit("waitid");

                /* Print status in hex, and as separate decimal bytes */
                printf("waitid() returned: PID=%ld; status=%d\n",
                       (long)childPid, si.si_status);

                printWaitStatus("hey", &si);

                if (si.si_code == CLD_EXITED || si.si_code == CLD_KILLED)
                    exit(EXIT_SUCCESS);
            }
    }
}
