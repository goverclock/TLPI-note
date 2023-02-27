// 用来向某一进程传递一系列信号

#include "tlpi_hdr.h"
#include <linux/limits.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    if (argc != 2) usageErr("%s pid\n", argv[0]);

    pid_t pid = atoi(argv[1]);
    for (int i = 1; i < 32; i++) {
        if (i == SIGKILL || i == SIGSTOP || i == SIGQUIT) continue;
        if (kill(pid, i) == -1 && errno != EINVAL) errExit("kill %d", i);
    }

    for (int i = SIGRTMIN; i <= SIGRTMAX; i++) {
        if (i == SIGKILL || i == SIGSTOP || i == SIGQUIT) continue;
        if (kill(pid, i) == -1 && errno != EINVAL) errExit("kill %d", i);
    }

    exit(EXIT_SUCCESS);
}
