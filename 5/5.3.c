/* 本习题的设计目的在于展示为何以 O_APPEND 标志打开文件来保障操作的原子性
是必要的。请编写一程序，可接收多达 3 个命令行参数：
$ atomic_append filename num-bytes [x]
 该程序应打开所指定的文件（如有必要，则创建之），然后以每次调用 write()写入一个
字节的方式，向文件尾部追加 num-bytes 个字节。缺省情况下，程序使用 O_APPEND
标志打开文件，但若存在第三个命令行参数（x），那么打开文件时将不再使用
O_APPEND 标志，代之以在每次调用 write()前调用 lseek(fd,0,SEEK_END)。同时运行
该程序的两个实例，不带 x 参数，将 100 万个字节写入同一文件：
$ atomic_append f1 1000000 & atomic_append f1 1000000
 重复上述操作，将数据写入另一文件，但运行时加入 x 参数：
$ atomic_append f2 1000000 & atomic_append f2 1000000 x
 使用 ls-1 命令检查文件 f1 和 f 2 的大小，并解释两文件大小不同的原因。
 */

/*
最开始,得到的运行结果为:
-rw-rw-r-- 1 ubuntu ubuntu 1999962 Jan 15 16:43 f1  (O_APPEND方式)
-rw-rw-r-- 1 ubuntu ubuntu 1579974 Jan 15 16:44 f2  (lseek方式)
最后发现是因为我在open中加了O_TRUNC,导致后运行的进程把先运行的进程写入的少量数据截掉了.
把这个标志位去掉就正常了:
-rw-rw-r-- 1 ubuntu ubuntu 2000000 Jan 15 17:05 f1
-rw-rw-r-- 1 ubuntu ubuntu 1644646 Jan 15 17:05 f2
可见O_APPEND标志位确实保证了设置偏移量和写数据的原子性.
 */

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "%s filename num-bytes [x]\n", argv[0]);
        return -1;
    }

    int num_bytes = atoi(argv[2]);
    int fd = -1;
    if (argc > 3) {
        fd = open(argv[1], O_WRONLY | O_CREAT, 0666);  // not atomic
        printf("not atomic\n");
    } else {
        fd = open(argv[1], O_WRONLY | O_CREAT | O_APPEND,
                  0666);  // atomic
        printf("atomic\n");
    }
    if (fd == -1) {
        errExit("open");
    }

    int num_write;
    printf("write %d bytes\n", num_bytes);
    while (num_bytes--) {
        if (argc > 3) {  // not atomic
            int pos = lseek(fd, 0, SEEK_END);
            if (pos == -1) {
                errExit("lseek");
            }
        }  // else atomic
        num_write = write(fd, "a", 1);
        if (num_write == -1) {
            errExit("write");
        }
    }

    return 0;
}
