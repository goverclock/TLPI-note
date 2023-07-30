/* 编写一个与 nice(1)命令类似的实时调度程序 set-user-ID-root 程序。这个程序的命令
行界面如下所示：
# ./rtsched policy priority command arg...
在上面的命令中，policy 中 r 表示 SCHED_RR，f 表示 SCHED_FIFO。基于在 9.7.1
节和 38.3 节中描述的原因，这个程序在执行命令前应该丢弃自己的特权 ID。 */

/* 
set-user-ID-root程序即root用户所拥有的set-user-ID程序,这意味着进程运行时会拥有特权.
这样的进程可以使用下面的方式放弃特权:
if (setuid(getuid()) == -1) {
    errExit("setuid");
}

而之所以最初需要特权,是因为sched_setscheduler()需要,然而书上对这一点的描述很模糊.

$ sudo ./a.out f 11 sleep 100 &
[1] 12687       # 注意,这个进程ID并不是下面的程序的,也许是sudo进程的
$ 12689         # 这个才是下面的程序,输出了自己的进程ID

$ ./sched_view 12689
12689: FIFO  11
 */

#include "tlpi_hdr.h"
#include <sched.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        usageErr("%s policy priority command arg...\n", argv[0]);
    }

    errno = 0;
    int prio = strtol(argv[2], NULL, 10);
    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    struct sched_param sp;
    sp.sched_priority = prio;
    if (argv[1][0] == 'r') {
        if (sched_setscheduler(0, SCHED_RR, &sp) == -1) {
            errExit("sched_setscheduler");
        }
    } else if (argv[1][0] == 'f') {
        if (sched_setscheduler(0, SCHED_FIFO, &sp) == -1) {
            errExit("sched_setscheduler");
        }
    } else {
        fprintf(stderr, "what?\n");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", getpid());
    // pid_t pid = fork();
    // if (pid == 0) {     // child
        execvp(argv[3], argv + 3);
        puts(strerror(errno));
    // }
    // printf("child pid=%d\n", pid);

    exit(EXIT_SUCCESS);
}
