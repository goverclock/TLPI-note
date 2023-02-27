/* 22.2 节曾指出，假设进程为 SIGCONT 信号建立了处理器函数并将其阻塞，如果该
进程已停止（stopped）后因收到一个 SIGCONT 信号而恢复执行，那么仅当解除了
对 SIGCONT 信号的阻塞时才会去调用信号处理器函数。编写一个程序来验证这一
点。回忆一下，按下终端暂停字符（通常为 Control-Z）可以停止进程，使用 kill-CONT
命令（或者隐蔽一点，使用 shell 的 fg 命令）可以发送 SIGCONT 信号。 */

/* 
有一些信号的处理是比较特殊的,例如SIGCONT信号,即使阻塞了它,停止的进程仍然会因为它继续执行.
书上的两段原文:

将该信号发送给已停止的进程，进程将会恢复运行（即在之后某个时间点重新获得调度）。
当接收信号的进程当前不处于停止状态时，默认情况下将忽略该信号。进程可以捕获该信号，
以便在恢复运行时可以执行某些操作。关于该信号的更多细节请参考 22.2 节和 34.7 节。

如果一个进程处于停止状态，那么一个 SIGCONT 信号的到来总是会促使其恢复运行，即
使该进程正在阻塞或者忽略 SIGCONT 信号。该特性之所以必要，是因为如果要恢复这些处于
停止状态的进程，舍此之外别无他法。（如果处于停止状态的进程正在阻塞 SIGCONT 信号，并且
已经为 SIGCONT 信号建立了处理器函数，那么在进程恢复运行后，只有当取消了对 SIGCONT
的阻塞时，进程才会去调用相应的处理器函数。）

每当进程收到 SIGCONT 信号时，会将处于等待状态的停止信号丢弃（即进程根本不知道
这些信号）。相反，如果任何停止信号传递给了进程，那么进程将自动丢弃任何处于等待状态
的 SIGCONT 信号。之所以采取这些步骤，意在防止之前发送的一个停止信号会在随后撤销
SIGCONT 信号的行为，反之亦然。

另外两个特殊的信号是SIGKILL和SIGSTOP,既不能改变其默认行为,也不能将其阻塞.

 */

#include "tlpi_hdr.h"
#include <signal.h>

void handler(int sig) {
    // unsafe
    printf("caught %d - %s\n", sig, strsignal(sig));
}

int main() {
    // set SIGCONT handler
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCONT, &act, NULL);

    // block SIGCONT
    sigset_t mask, prev;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCONT);
    if (sigprocmask(SIG_BLOCK, &mask, &prev) == -1) errExit("sigprocmask 1");

    puts("set handler and blocked SIGCONT");
    puts("now press Ctrl-z...");
    
    while (sleep(10)) continue;     // sleep until a signal arrives which is not ignored
                                    // in this case SIGSTOP

    puts("process continued, sleep 5s");
    sleep(5);
    puts("unblock SIGCONT");
    if (sigprocmask(SIG_SETMASK, &prev, NULL) == -1) errExit("sigprocmask 2");

    exit(EXIT_SUCCESS);
}

