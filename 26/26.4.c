/* 程序清单 26-4（make_zombie.c）调用了 sleep()，以便允许子进程在父进程执行函
数 system()前得到机会去运行并终止。这一方法理论上存在产生竞争条件的可能。
修改此程序，使用信号来同步父子进程以消除该竞争条件。 */

/*
使用sigwaitinfo来等待SIGCHLD来实现同步.
 */

#include "tlpi_hdr.h"
#include <signal.h>
#include <libgen.h> /* For basename() declaration */

#define CMD_SIZE 200

int main(int argc, char *argv[]) {
    char cmd[CMD_SIZE];
    pid_t childPid;

    setbuf(stdout, NULL); /* Disable buffering of stdout */

    printf("Parent PID=%ld\n", (long)getpid());

    sigset_t mask, orig;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &mask, &orig) == -1) errExit("sigprocmask");

    switch (childPid = fork()) {
        case -1:
            errExit("fork");

        case 0: /* Child: immediately exits to become zombie */
            printf("Child (PID=%ld) exiting\n", (long)getpid());
            _exit(EXIT_SUCCESS);

        default:      /* Parent */
            sigwaitinfo(&mask, NULL); /* Give child a chance to start and exit */
            snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
            system(cmd); /* View zombie child */

            /* Now send the "sure kill" signal to the zombie */

            if (kill(childPid, SIGKILL) == -1) errMsg("kill");
            sleep(3); /* Give child a chance to react to signal */
            printf("After sending SIGKILL to zombie (PID=%ld):\n",
                   (long)childPid);
            system(cmd); /* View zombie child again */

            exit(EXIT_SUCCESS);
    }
}
