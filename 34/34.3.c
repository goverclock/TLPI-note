/* 编写一个程序来验证在进程组首进程中调用 setsid()会失败。 */

/* 
书中34.3节对于这条约束的解释如下:

约束进程组首进程对 setsid()的调用是有必要的。因为如果没有这个约束的话，进程组组长就
能够将其自身迁移至另一个（新的）会话中了，而该进程组的其他成员则仍然位于原来的会话中。
（不会创建一个新进程组，因为根据定义，进程组首进程的进程组 ID 已经与其进程 ID 一样了。）
这会破坏会话和进程组之间严格的两级层次，因此一个进程组的所有成员必须属于同一个会话。

还提到了一种绕过此种约束的方法:

避免这个错
误发生的最简单的方式是执行一个 fork()并让父进程终止以及让子进程调用 setsid()。由于子进程
会继承其父进程的进程组 ID 并接收属于自己的唯一的进程 ID，因此它无法成为进程组首进程。

运行结果:

[gopi:34]$ ./a.out 
Operation not permitted
[gopi:34]$ ls | ./a.out     # 避免成为进程组首进程
succeed 29934
 */

#include "tlpi_hdr.h"
#include <unistd.h>

int main() {
    pid_t ret = setsid();
    if (ret == -1) {
        puts(strerror(errno));
    } else {
        printf("succeed %d\n", ret);
    }

    exit(EXIT_SUCCESS);
}
