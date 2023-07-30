/* 如果两个进程在一个多处理器系统上使用管道来交换大量数据，那么两个进程
运行在同一个 CPU 上的通信速度应该要快于两个进程运行在不同的 CPU 上，
其原因是当两个进程运行在同一个 CPU 上时能够快速地访问管道数据，因为
管道数据可以保留在 CPU 的高速缓冲器中。相反，当两个进程运行在不同的
CPU 上时将无法享受 CPU 高速缓冲器带来的优势。读者如果拥有多处理器系
统，可以编写一个使用 sched_setaffinity()强制将两个进程运行在同一个 CPU 上
或运行在两个不同的 CPU 上的程序来演示这种效果。（第 44 章描述了管道的
使用。） */

/* 
为了保证两个进程运行在不同的CPU上,需要分别设置亲和力到一个单独的CPU.

$ ./a.out 
same CPU
1.360000
$ ./a.out x
different CPU
1.420000 
*/

#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include <sys/times.h>
#include <sched.h>

int main(int argc, char *argv[]) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    sched_setaffinity(0, sizeof(set), &set);

    int p[2];
    pipe(p);

    int t = 1000000;
    struct tms bg;
    struct tms cur;
    switch(fork()) {
        case -1:
            errExit("fork");
        case 0:     // child, may be on CPU 0 or 1
            if (argc > 1) {
                puts("different CPU");
                CPU_ZERO(&set);
                CPU_SET(1, &set);
                sched_setaffinity(0, sizeof(set), &set);
            } else {
                puts("same CPU");
            }
        
            if (close(p[1]) == -1)
                errExit("close");
            char buf[26];

            times(&bg);
            for (int i = 0; i < t; i++)
                read(p[0], buf, 26);

            times(&cur);
            double bg_sec = ((double)bg.tms_utime + (double)bg.tms_stime) / sysconf(_SC_CLK_TCK);
            double cur_sec = ((double)cur.tms_utime + (double)cur.tms_stime) / sysconf(_SC_CLK_TCK);
            printf("%lf\n", cur_sec - bg_sec);
            break;
        default:    // parent, always on CPU 0
            if (close(p[0]) == -1)
                errExit("close");
            for (int i = 0; i < t; i++)
                write(p[1], "qwertyuiopasdfghjklzxcvbnm", 26);
    }

    exit(EXIT_SUCCESS);
}
