/* 编写一个程序来执行一个命令，接着显示其当前的资源使用。这个程序与 time(1)
命令的功能类似，因此可以像下面这样使用这个程序： 
    $ ./rusage command arg...*/

/* 
和35.2类似,用execvp().
 */

#include "tlpi_hdr.h"
#include <sys/resource.h>
#include <sys/wait.h>

// from print_rusage.c
void printRusage(const char *leader, const struct rusage *ru) {
    const char *ldr;

    ldr = (leader == NULL) ? "" : leader;

    printf("%sCPU time (secs):         user=%.3f; system=%.3f\n", ldr,
           ru->ru_utime.tv_sec + ru->ru_utime.tv_usec / 1000000.0,
           ru->ru_stime.tv_sec + ru->ru_stime.tv_usec / 1000000.0);
    printf("%sMax resident set size:   %ld\n", ldr, ru->ru_maxrss);
    printf("%sIntegral shared memory:  %ld\n", ldr, ru->ru_ixrss);
    printf("%sIntegral unshared data:  %ld\n", ldr, ru->ru_idrss);
    printf("%sIntegral unshared stack: %ld\n", ldr, ru->ru_isrss);
    printf("%sPage reclaims:           %ld\n", ldr, ru->ru_minflt);
    printf("%sPage faults:             %ld\n", ldr, ru->ru_majflt);
    printf("%sSwaps:                   %ld\n", ldr, ru->ru_nswap);
    printf("%sBlock I/Os:              input=%ld; output=%ld\n", ldr,
           ru->ru_inblock, ru->ru_oublock);
    printf("%sSignals received:        %ld\n", ldr, ru->ru_nsignals);
    printf("%sIPC messages:            sent=%ld; received=%ld\n", ldr,
           ru->ru_msgsnd, ru->ru_msgrcv);
    printf(
        "%sContext switches:        voluntary=%ld; "
        "involuntary=%ld\n",
        ldr, ru->ru_nvcsw, ru->ru_nivcsw);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s command arg...\n", argv[0]);

    pid_t pid;
    switch (pid = fork()) {
        case -1:
            errExit("fork");
        case 0:         // child
            execvp(argv[1], argv + 1);
            errExit("execvp");
        default:        // parent
            printf("Command PID: %ld\n", (long)pid);
            if (wait(NULL) == -1) errExit("wait");

            struct rusage ru;
            if (getrusage(RUSAGE_CHILDREN, &ru) == -1) errExit("getrusage");
            puts("");
            printRusage("\t", &ru);
    }
    exit(EXIT_SUCCESS);
}
