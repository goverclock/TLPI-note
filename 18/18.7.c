/* 编写一程序，使用 nftw()来遍历目录树，并打印出树中各类文件（普通文件、目录、
符号链接等）的总和及百分比. */

/*
这里的百分比指的是各类文件的数量占比.
 */

#define _XOPEN_SOURCE 600
#include <ftw.h>
#include <sys/stat.h>
#include "18.8.c"

#include "tlpi_hdr.h"

static int num_blk = 0, num_chr = 0, num_dir = 0, num_fifo = 0, num_link = 0,
           num_reg = 0, num_sock = 0, num_unknown = 0, num_nonstatable = 0;

int count_file(const char *fpath, const struct stat *sb, int typeflag,
               struct FTW *ftwbuf);

int main(int argc, char *argv[]) {
    if (argc != 2) usageErr("%s file\n", argv[0]);
    if (my_nftw(argv[1], count_file, 20, FTW_PHYS) == -1) errExit("fuck");
    int total = num_blk + num_chr + num_dir + num_fifo + num_link + num_reg +
                num_sock + num_unknown + num_nonstatable;

    printf("total:      %d\n", total);
    printf("blk:        %.2f%%\n", (num_blk * 100.0) / total);
    printf("chr:        %.2f%%\n", (num_chr * 100.0) / total);
    printf("dir:        %.2f%%\n", (num_dir * 100.0) / total);
    printf("fifo:       %.2f%%\n", (num_fifo * 100.0) / total);
    printf("link:       %.2f%%\n", (num_link * 100.0) / total);
    printf("reg:        %.2f%%\n", (num_reg * 100.0) / total);
    printf("sock:       %.2f%%\n", (num_sock * 100.0) / total);
    printf("unknown:    %.2f%%\n", (num_unknown * 100.0) / total);
    printf("nonstatable:%.2f%%\n", (num_nonstatable * 100.0) / total);

    return 0;
}

int count_file(const char *fpath, const struct stat *sb, int typeflag,
               struct FTW *ftwbuf) {
    if (typeflag == FTW_NS) {
        num_nonstatable++;
        return 0;
    }

    // copyed from man 2 stat
    switch (sb->st_mode & S_IFMT) {
        case S_IFBLK:
            num_blk++;
            break;
        case S_IFCHR:
            num_chr++;
            break;
        case S_IFDIR:
            num_dir++;
            break;
        case S_IFIFO:
            num_fifo++;
            break;
        case S_IFLNK:
            num_link++;
            break;
        case S_IFREG:
            num_reg++;
            break;
        case S_IFSOCK:
            num_sock++;
            break;
        default:
            num_unknown++;
            break;
    }
    return 0;
}
