/* 编写一程序来展示当将对等待信号的处置改为 SIG_IGN 时，程序绝不会看到（捕
获）信号。 */

/* 
sigaction无法修改对SIGKILL和SIGSTOP信号的行为.必杀信号和必停信号无法被捕获,阻塞或者忽略,
总能终止/停止进程.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/20$ ./20.2
^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^C^Cubuntu@VM-4-12-ubuntu:~/TLPI-note/20$ ^C

 */

#include "tlpi_hdr.h"
#include <signal.h>

int main() {
    int sig = SIGINT;   // can't be SIGKILL or SIGSTOP

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = SIG_IGN;
    if (sigaction(sig, &act, NULL) == -1) errExit("sigaction");
    sleep(10);

    return 0;
}
