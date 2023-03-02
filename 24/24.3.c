/* 假设可以修改程序源代码，如何在某一特定时刻生成一核心转储（core dump）文
件，而同时进程得以继续执行？ */

/*
当然是fork()一个子进程,让子进程调用abort()了
这里发现一个现象,就是如果在子进程中调用abort(),则不会显示 Aborted (core dumped) 的信息.
而在父进程中是有这条信息的.
我做了一些测试,最后发现这条信息应该不是进程输出的,而是shell输出的提示:
- 如果在父进程abort()前关闭所有文件描述符,仍会出现上述信息
- 如果在shell中将stderr重定向到/dev/null,由fprintf(stderr, ...)输出的信息不显示,但上述信息
    仍会显示

所以我的结论是这条信息是shell检测到前台进程(也就是父进程)的异常退出后给出的提示.

书上的答案提到一点:

gdb gcore 命令为程序执行类似任务，且不需要修改源码。

 */

#include "tlpi_hdr.h"

int main() {
    // abort();
    puts("Hello");

    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:  // child
            abort();
        default:
            break;
    }

    exit(EXIT_SUCCESS);
}
