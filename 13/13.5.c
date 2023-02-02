/* tail [ –n num ] file 命令打印名为 file 文件的最后 num 行（默认为 10
行）。使用 I/O
系统调用（lseek()、read()、write()等）来实现该命令。牢记本章所描述的缓冲问题，
力求实现的高效性。 */

/*
使用lseek的时候要注意不要把第二和第三个参数填反了= =

注意这里要求用系统调用,而不是stdio.stdio缓冲区有典型值BUFSIZE=8192,
而书上对系统调用I/O的描述是:

从内核 2.4 开始，Linux 不再维护一个单独的缓冲区高速缓存。相反，会
将文件 I/O 缓冲区置于页面高速缓存中，其中还含有诸如内存映射文件的页面。

SO上的回答:

write system call normally just copies your data to the page cache, which
 later on gets flushed to the disk. The size of the page cache is dynamic,
 the kernel tries to use all free memory for it.

https://stackoverflow.com/questions/25041234/disk-write-buffer-size-in-linu

所以我就自己挑一个缓冲区大小了.

具体做法是从文件末尾开始以一个BUFFER_SIZE大小为单位往前逐个读取,直达发现的换行符数量足够.

性能当然是合理的,例如读取一个大文件:

ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ ls -l large_file 
-rwxrwxr-x 1 ubuntu ubuntu 9809119 Feb  3 06:24 large_file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ g++ ./13.5.c -o 13.5 -ltlpi -DBUFFER_SIZE=1024
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ time ./13.5 -n40 large_file 
asdfleqw
qhewrjkhasdf
hweqjrhkqwer



asdfjklqw
er
qwejrkljsalkdf
qjwerjo q23eri2=3\ri\\2
2 
hsqwlejhqw'e
qwe jqw
e ia
iwe
qiwei[1i2i3[i
as e
as
d
i12i d
ia wsid
a s
diqwi
 id
 1 wi

  iqw
   id iwa
   i 
   iw 


    aw


12

31
3


real    0m0.008s
user    0m0.007s
sys     0m0.001s

BUFFER_SIZE设置为1,读取最后4000行:
real    0m23.552s
user    0m1.661s
sys     0m5.481s

BUFFER_SIZE设置为8192,读取最后4000行:
real    0m4.062s
user    0m0.009s
sys     0m0.031s

 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "tlpi_hdr.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 8192
#endif

int get_block(int, void *);

int main(int argc, char *argv[]) {
    int opt;
    int num = 10;
    char *fname;
    while ((opt = getopt(argc, argv, ":n:")) != -1) {
        if (opt == '?' || opt == ':') {
            usageErr("%s [-n num] file\n", argv[0]);
            exit(-1);
        } else if (opt == 'n') {
            num = strtol(optarg, NULL, 10);
        }
    }
    if (argc != 2 && argc != 3 && argc != 4) {
        usageErr("%s [-n num] file\n", argv[0]);
        exit(-1);
    }
    if (num <= 0) {
        puts("seriously?");
        exit(-1);
    }
    fname = argv[argc - 1];

    int fd = -1;
    if ((fd = open(fname, O_RDONLY)) == -1) {
        errExit("open");
    }

    char buf[BUFFER_SIZE];
    int num_read;
    int nl_cnt = 0;
    while (num_read = get_block(fd, buf)) {
        for (int i = num_read - 1; i >= 0; i--) {
            if (buf[i] == '\n') {
                nl_cnt++;
                if (nl_cnt == num) {
                    if (lseek(fd, i - num_read + 1, SEEK_CUR) == -1) errExit("lseek");
                    break;
                }
            }
        }
        if (nl_cnt == num) break;
    }
    if (nl_cnt < num) {
        if (lseek(fd, 0, SEEK_SET) == -1) errExit("lseek");
    }

    while((num_read = read(fd, buf, BUFFER_SIZE)) > 0) 
        write(STDOUT_FILENO, buf, num_read);
    write(STDOUT_FILENO, "\n", 1);
    
    return 0;
}

// read a block from backwards in buf
// return bytes read
// offset is at the end of buf in file
int get_block(int fd, void *buf) {
    static int blk_cnt = 1;
    static off_t end = -1;
    if (end == -1) {
        end = lseek(fd, 0, SEEK_END);
    }

    errno = 0;
    off_t pos = lseek(fd, -blk_cnt * BUFFER_SIZE, SEEK_END);
    if (pos == -1) {
        if (errno == EINVAL) {  // offset would be negative
            errno = 0;
            int num_to_read = lseek(fd, -(blk_cnt - 1) * BUFFER_SIZE, SEEK_END);
            blk_cnt++;
            if (num_to_read == -1 && errno == EINVAL) return 0;
            if (num_to_read > end) num_to_read = end;
            if (lseek(fd, 0, SEEK_SET) == -1) errExit("lseek");
            return read(fd, buf, num_to_read);
        } else
            errExit("lseek");
    }

    blk_cnt++;
    return read(fd, buf, BUFFER_SIZE);
}
