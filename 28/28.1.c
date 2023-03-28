/* 编写一程序，观察 fork()和 vfork()系统调用在读者系统中的速度。要求每个子进程
必须立即退出，而父进程应在创建下一个子进程之前调用 wait()，等待当前子进程
退出。将两个系统调用之间的差别与表 28-3 相比较。shell 内建命令 time 可用来测
量程序的执行时间。 */


/* 
与书中的测试结果一致,vfork比fork要快.
这和我预想的结果不一样,我想的是既然vfork后父进程要阻塞到子进程退出才会继续,那应该会比fork更慢.
但实际上不管是vfork还是fork,在这个程序里面都会在子进程退出前阻塞(wait调用).
而调用vfork是不会复制页表或页的,fork却需要为子进程复制页表,以及将数据段,堆段和栈段的页标记为只读.
所以两者耗费时间的差值其实就是复制进程页表所需的时间.

另外,书上的表格还有另一个结论,随着子进程占用的虚拟内存总量增大,fork需要复制的页表项目也更多,
这使得fork会花费更多的时间.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/28$ time ./28.1 
argc = 1, using fork

real    0m13.940s
user    0m8.176s
sys     0m5.629s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/28$ time ./28.1 1
argc > 1, using vfork

real    0m4.698s
user    0m2.296s
sys     0m3.216s 

*/



#include "tlpi_hdr.h"
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc > 1)
        puts("argc > 1, using vfork");
    else
        puts("argc = 1, using fork");

    int child_cnt = 100000;
    if (argc > 1)
        while (child_cnt--) {
            switch (vfork()) {
                case -1:
                    errExit("fork");
                case 0:  // child exit immidiately
                    exit(EXIT_SUCCESS);
                default:  // parent
                    wait(NULL);
            }
        }
    else
        while (child_cnt--) {
            switch (fork()) {
                case -1:
                    errExit("fork");
                case 0:  // child exit immidiately
                    exit(EXIT_SUCCESS);
                default:  // parent
                    wait(NULL);
            }
        }

    exit(EXIT_SUCCESS);
}
