/* 编写一个程序来测量管道的带宽。在命令行参数中，程序应该接收需发送的数据块
数目以及每个数据块的大小。在创建一个管道之后，程序将分成两个进程：一个子
进程以尽可能快的速度向管道写入数据块，父进程读取数据块。在所有数据都被读
取之后，父进程应该打印出所消耗的时间和带宽（每秒传输的字节数）。为不同的
数据块大小测量带宽。 */

/* 

 */

#include "tlpi_hdr.h"
#include <sys/time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        usageErr("%s number size\n", argv[0]);
    }
    unsigned long long n = getLong(argv[1], 0, "number");
    unsigned long long sz = getLong(argv[2], 0, "size");

    struct timeval bg, ed;
    double elapsed;
    unsigned long long transfered = 0;

    int p[2];
    if (pipe(p) == -1) {
        errExit("pipe");
    }

    char *buf = (char *)malloc(sz);
    memset(buf, 'a', sz);

    gettimeofday(&bg, NULL);
    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:  // child writes to pipe
            close(p[0]);
            while (transfered < n * sz) {
                int ret = write(p[1], buf, sz);
                transfered += ret;
                if (ret == -1) {
                    errExit("child write");
                }
            }
            exit(EXIT_SUCCESS);
        default:  // parent reads from pipe, then fall through
            close(p[1]);
            while (transfered < n * sz) {
                int ret = read(p[0], buf, sz);
                transfered += ret;
                if (ret == -1) {
                    errExit("parent read");
                }
            }
    }

    gettimeofday(&ed, NULL);

    elapsed =
        (ed.tv_sec - bg.tv_sec) * 1000.0 + (ed.tv_usec - bg.tv_usec) / 1000.0;
    printf("transfered:\t%llu byte\n", transfered);
    printf("elapsed:\t%f ms\n", elapsed);
    printf("bandwidth:\t%d byte/ms\n", transfered / (int)elapsed);

    exit(EXIT_SUCCESS);
}
