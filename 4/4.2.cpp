/* 编写一个类似于 cp 命令的程序，当使用该程序复制一个包含空洞（连续的空字节）
的普通文件时，要求目标文件的空洞与源文件保持一致。 */

#include <fcntl.h>
#include <unistd.h>

#include "tlpi_hdr.h"

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        usageErr("%s oldfile newfile\n", argv[0]);
    }

    int old_fd, new_fd;
    old_fd = open(argv[1], O_RDONLY);
    if (old_fd == -1) {
        errExit("open");
    }
    // make sure newfile doesn't exist and we create it
    // so that we don't overwrite an existing file
    new_fd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);   // -wr-wr-wr
                  // same as open(..., ..., 0666)
    if (new_fd == -1) {
        errExit("open");
    }    

    char *buf[BUF_SIZE];
    int num_read;
    while ((num_read = read(old_fd, buf, BUF_SIZE)) > 0) {
        write(new_fd, buf, num_read);
    }

    return 0;
}
