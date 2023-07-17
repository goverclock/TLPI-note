/* 编写一个程序来验证父进程能够在子进程执行 exec()之前修改子进程的进程组 ID，
但无法在执行 exec()之后修改子进程的进程组 ID。 */

/*
书中34.2节原文对于这条约束的解释:
一个进程在其子进程已经执行 exec()后就无法修改该子进程的进程组 ID 了。违反这条
规则会导致 EACCES 错误。之所以会有这条约束条件的原因是在一个进程开始执行之
后再修改其进程组 ID 的话会使程序变得混乱。

这段程序根据argc决定是否在setpgid()执行exec(),两种情况下的执行结果如下:

[gopi:34]$ gcc ./34.2.c -ltlpi; ./a.out
/dev/null
fail
Permission denied
[gopi:34]$ gcc ./34.2.c -ltlpi; ./a.out x
succeed
[gopi:34]$ /dev/null

*/

#include "tlpi_hdr.h"
#include <unistd.h>

int main(int argc, char *argv) {
    pid_t pid;
    switch (pid = fork()) {
        case -1:
            errExit("fork");
        case 0:     // child
            if (argc > 1) {
                sleep(5);
            }
            execl("/usr/bin/ls", "ls", "/dev/null", NULL);
            break;
        default:    // parent
            sleep(2);
            if (setpgid(pid, pid) == -1) {
                puts("fail");
                puts(strerror(errno));
            } else {
                puts("succeed");
            }
    }

    exit(EXIT_SUCCESS);
}
