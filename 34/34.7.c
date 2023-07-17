/* 编写一个程序来验证当 SIGTTIN、SIGTTOU 或 SIGTSTP 三个信号中的一个信号被
发送给孤儿进程组中的一个成员时，如果这个信号会停止该进程（即处理方式为
SIG_DFL），那么这个信号就会被丢弃（即不产生任何效果），但如果该信号存在处
理器，就会发送该信号。 */

/* 
$ ./a.out           # 子进程处在孤儿进程组
$ child quitting    # SIGTTIN信号被丢弃

$ ./a.out x         # 子进程没有处在孤儿进程组
$                   # SIGTTIN信号使子进程停止
 */

#include "tlpi_hdr.h"
#include <signal.h>

void handler(int sig) {
    // unsafe
    printf("caught %d - %s\n", sig, strsignal(sig));
}

int main(int argc, char *argv[]) {
    // struct sigaction sa;
    // sigemptyset(&sa.sa_mask);
    // sa.sa_handler = handler;
    // if (sigaction(SIGTTIN, &sa, NULL) == -1) {
    //     errExit("sigaction");
    // }
    
    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:     // child
            sleep(2);
            raise(SIGTTIN);
            puts("child quitting");
            
            break;
        default:    // parent
            if (argc > 1) {
                sleep(5);  
            }   // else the child process goes into a orphan pg
    }

    exit(EXIT_SUCCESS);
}
