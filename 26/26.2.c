/* 假设存在 3 个相互关联的进程（祖父、父及子进程），祖父进程没有在父进程退出
之后立即执行 wait()，所以父进程变成僵尸进程。那么请指出孙进程何时被 init 进程
收养（即孙进程调用 getppid()将返回 1），是在父进程终止后，还是祖父进程调用
wait()后？请编写程序验证结果。 */

/* 
结果是在父进程终止后子进程就被init收养了.
书上原话:某一子进程的父进程终止后,对getppid()的调用将返回1.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/26$ ./26.2
grandparent: pid=2912988
parent: pid=2912989
child: pid=2912990
child: ppid=2912989
parent: exit
child: ppid=1
grand parent: exit
 */

#include "tlpi_hdr.h"

void child() {
    pid_t cpid = getpid();
    printf("child: pid=%d\n", cpid);
    printf("child: ppid=%d\n", getppid());
    sleep(5);
    printf("child: ppid=%d\n", getppid());
    _exit(EXIT_SUCCESS);
}

void parent() {
    pid_t ppid = getpid();
    printf("parent: pid=%d\n", ppid);
    switch(fork()) {
        case -1:
            errExit("fork");
        case 0:     // child
            return child();
        default:
            break;
    }
    sleep(3);
    printf("parent: exit\n");

    _exit(EXIT_SUCCESS);
}

int main() {
    setbuf(stdout, NULL);
    pid_t gppid = getpid();
    printf("grandparent: pid=%d\n", gppid);

    switch(fork()) {
        case -1:
            errExit("fork");
        case 0:
            parent();    // calls _exit()
        default:        // grand parent
            break;
    }

    sleep(10);
    puts("grand parent: exit");

    exit(EXIT_SUCCESS);
}
