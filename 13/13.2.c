/* 测定 filebuff/write_bytes.c（随本书发行版提供源码）程序在不同的缓冲区大小以及
文件系统下的用时。 */

/* 
没有设置额外的编译选项,结果和13.1类似.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ ./13.2 a 10000000 32
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ time ./13.2 a 10000000 32

real    0m0.666s
user    0m0.090s
sys     0m0.469s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ time ./13.2 a 10000000 2

real    0m11.738s
user    0m1.226s
sys     0m7.447s
 */

/* Supplementary program for Chapter 13 */

/* write_bytes.c

   Write bytes to a file. (A simple program for file I/O benchmarking.)

   Usage: write_bytes file num-bytes buf-size

   Writes 'num-bytes' bytes to 'file', using a buffer size of 'buf-size'
   for each write().

   If compiled with -DUSE_O_SYNC, open the file with the O_SYNC flag,
   so that all data and metadata changes are flushed to the disk.

   If compiled with -DUSE_FDATASYNC, perform an fdatasync() after each write,
   so that data--and possibly metadata--changes are flushed to the disk.

   If compiled with -DUSE_FSYNC, perform an fsync() after each write, so that
   data and metadata are flushed to the disk.
*/
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    if (argc != 4 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file num-bytes buf-size\n", argv[0]);

    size_t numBytes = getLong(argv[2], GN_GT_0, "num-bytes");
    size_t bufSize = getLong(argv[3], GN_GT_0, "buf-size");

    char *buf = (char*)malloc(bufSize);
    if (buf == NULL)
        errExit("malloc");

    int openFlags = O_CREAT | O_WRONLY;

#if defined(USE_O_SYNC) && defined(O_SYNC)
    openFlags |= O_SYNC;
#endif

    int fd = open(argv[1], openFlags, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("open");

    size_t thisWrite, totWritten;
    for (totWritten = 0; totWritten < numBytes;
            totWritten += thisWrite) {
        thisWrite = min(bufSize, numBytes - totWritten);

        if (write(fd, buf, thisWrite) != thisWrite)
            fatal("partial/failed write");

#ifdef USE_FSYNC
        if (fsync(fd))
            errExit("fsync");
#endif
#ifdef USE_FDATASYNC
        if (fdatasync(fd))
            errExit("fdatasync");
#endif
    }

    if (close(fd) == -1)
        errExit("close");
    exit(EXIT_SUCCESS);
}
