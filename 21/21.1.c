/* 实现abort(). */

/* 
来自书上的对于abort()的介绍:

函数 abort()通过产生 SIGABRT 信号来终止调用进程。对 SIGABRT 的默认动作是产
生核心转储文件并终止进程。调试器可以利用核心转储文件来检测调用 abort()时的程序
状态。
SUSv3 要求，无论阻塞或者忽略 SIGABRT 信号，abort()调用均不受影响。同时规定，除
非进程捕获 SIGABRT 信号后信号处理器函数尚未返回，否则 abort()必须终止进程。后一句话
值得三思。21.2 节所描述的信号处理器函数终止方法中，与此相关的就是使用非本地跳转退
出处理器函数。这一做法将抵消 abort()的效果。否则，abort()将总是终止进程。在大多数实现
中，终止时可确保发生如下事件：若进程在发出一次 SIGABRT 信号后仍未终止（即，处理器
捕获信号并返回，以便恢复执行 abort()），则 abort()会将对 SIGABRT 信号的处理重置为
SIG_DFL，并再度发出 SIGABRT 信号，从而确保将进程杀死。
如果 abort()成功终止了进程，那么还将刷新 stdio 流并将其关闭。 

所以实现abort()其实很容易,只需要先取消对SIGABRT信号的阻塞,发出(raise())一次SIGABRT信号,然后
重置对SIGABRT信号的处置,然后再次发出SIGABRT.

也可以参考abort(3)的描述:
The  abort() function first unblocks the SIGA‐
BRT signal, and then raises  that  signal  for
the  calling  process  (as though raise(3) was
called).  This results in the abnormal  termi‐
nation  of the process unless the SIGABRT sig‐
nal is caught and the signal handler does  not
return (see longjmp(3)).

If the SIGABRT signal is ignored, or caught by
a handler that returns, the  abort()  function
will  still  terminate  the  process.  It does
this by restoring the default disposition  for
SIGABRT and then raising the signal for a sec‐
ond time.

abort()不需要负责生成核心转储文件,这是内核的任务.

此外,abort(3)提示刷新stdio流并将其关闭的行为已经是不必的了:

NOTES
       Up  until  glibc 2.26, if the abort() function
       caused process termination, all  open  streams
       were  closed  and flushed (as with fclose(3)).
       However, in some cases this  could  result  in
       deadlocks  and  data  corruption.   Therefore,
       starting with glibc 2.27,  abort()  terminates
       the process without flushing streams.  POSIX.1
       permits either possible behavior, saying  that
       abort()  "may  include  an  attempt  to effect
       fclose() on all open streams"

*/

#include "tlpi_hdr.h"
#include <signal.h>

void my_abort() {
    // 1. unblock SIGABRT
    sigset_t ss;
    if (sigemptyset(&ss) == -1) errExit("sigemptyset");
    if (sigaddset(&ss, SIGABRT) == -1) errExit("sigaddset");
    if (sigprocmask(SIG_UNBLOCK, &ss, NULL) == -1) errExit("sigprocmask");
    
    // 2. raise SIGABRT
    if (raise(SIGABRT) != 0) errExit("raise 1");

    // 3. now the SIGABRT must have been caught or ignored, 
    // restore the default disposition for SIGABRT
    struct sigaction act;
    act.sa_handler = SIG_DFL;
    if (sigaction(SIGABRT, &act, NULL) == -1) errExit("sigaction");

    // 4. raise SIGABRT again
    if (raise(SIGABRT) != 0) errExit("raise 2");
}

int main() {
    my_abort();


    return 0;
}
