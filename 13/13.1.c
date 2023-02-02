/* 使用 shell 内嵌的 time 命令，测算程序清单 4-1(copy.c)在当前环境下的用时。
a ）使用不同的文件和缓冲区大小进行试验。编译应用程序时使用
–DBUF_SIZE=nbytes 选项可设置缓冲区大小。
b）对 open()的系统调用加入 O_SYNC 标识，针对不同大小的缓冲区，速度存在多
大差异？
c） 在一系列文件系统（比如，ext3、XFS、Btrfs 和 JFS）中执行这些计时测试。
结果相似吗？当缓冲区大小从小变大时，用时趋势相同吗？ */

/* 
gcc的头文件路径似乎没设好,所以用g++了= =

a) 影响非常明显

ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ g++ ./13.1.c -o 13.1 -g -ltlpi -DBUF_SIZE=1024
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ time ./13.1 large_file new_large

real    0m0.070s
user    0m0.011s
sys     0m0.042s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ g++ ./13.1.c -o 13.1 -g -ltlpi -DBUF_SIZE=64
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ time ./13.1 large_file new_large

real    0m0.659s
user    0m0.081s
sys     0m0.392s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ g++ ./13.1.c -o 13.1 -g -ltlpi -DBUF_SIZE=8
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ time ./13.1 large_file new_large

real    0m4.494s
user    0m0.680s
sys     0m2.900s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ g++ ./13.1.c -o 13.1 -g -ltlpi -DBUF_SIZE=4
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ time ./13.1 large_file new_large

real    0m9.425s
user    0m1.308s
sys     0m5.792s

b) 影响非常非常明显,可以看到I/O次数减少到十分之一,导致耗时也减少到十分之一

ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ g++ ./13.1.c -o 13.1 -g -ltlpi -DBUF_SIZE=1024 -DSYNC=O_SYNC
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ time ./13.1 large_file new_large

real    0m32.159s
user    0m0.027s
sys     0m0.699s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ g++ ./13.1.c -o 13.1 -g -ltlpi -DBUF_SIZE=10240 -DSYNC=O_SYNC
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ time ./13.1 large_file new_large

real    0m3.459s
user    0m0.004s
sys     0m0.097s

c) pass

 */



// 4-1
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif

// 这地方是我加上去的,和BUF_SIZE类似,这样在编译选项里加入-DSYNC=O_SYNC就可以直接I/O了
// 当然也修改了openFlags那行的代码
#ifndef SYNC
#define SYNC 0
#endif

int
main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s old-file new-file\n", argv[0]);

    /* Open input and output files */

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1)
        errExit("opening file %s", argv[1]);

    openFlags = O_CREAT | O_WRONLY | O_TRUNC | SYNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- */
    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1)
        errExit("opening file %s", argv[2]);

    /* Transfer data until we encounter end of input or an error */

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
        if (write(outputFd, buf, numRead) != numRead)
            fatal("write() returned error or partial write occurred");
    if (numRead == -1)
        errExit("read");

    if (close(inputFd) == -1)
        errExit("close input");
    if (close(outputFd) == -1)
        errExit("close output");

    exit(EXIT_SUCCESS);
}
