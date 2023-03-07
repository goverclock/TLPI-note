/* 编写一程序以验证当一子进程的父进程终止时，调用 getppid()将返回 1（进程 init
的 进程 ID）。 */

/*
勘误表:
...on some modern init frameworks such as systemd, a dedicated process
(with PID other than 1) takes on the role of adopting orphaned children.
Thus, in code that solves the exercise, the PPID of the process will
change as the child becomes orphaned, but the change with be to a value
other than 1. For further details, see the erratum for page 553.
A similar qualification also applies for exercise 26-2.

也就是说如今的实现中收养孤儿进程的可能不是init进程,而是某个指定的进程.
不过在我的机器上还是由init来收养的.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/26$ ./26.1
child: ppid=2907391
parent: quit
ubuntu@VM-4-12-ubuntu:~/TLPI-note/26$ child: ppid=1

重提一下,应该把stdio的缓冲区禁用.否则的话输出到终端为行缓冲,输出到文件为块缓冲.
在这里每一个输出我都加了换行符,所以输出到终端时正常,输出到文件就会出错.和之前的某次练习的结论一样.

 */

#include "tlpi_hdr.h"

int main() {
    setbuf(stdout, NULL);
    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:  // child
            printf("child: ppid=%d\n", getppid());
            sleep(3);
            printf("child: ppid=%d\n", getppid());
            break;
        default:  // parent
            sleep(1);
            printf("parent: quit\n");
            break;
    }

    exit(EXIT_SUCCESS);
}
