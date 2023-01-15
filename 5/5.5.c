/* 编写一程序，验证文件描述符及其副本是否共享了文件偏移量和打开文件的状态
标志。 */

/* 
fcntl(fd, F_GETFL)返回文件描述符的flag参数
lseek(fd, SEEK_CUR)返回文件描述符的偏移量
dup和dup2都可以创建文件描述符的副本
注意用printf输出off_t类型时应该强制转化为long long后用%lld输出

$ ./5.5
old flags: 32769, new flags: 32769
old offset: 5, new offset: 5
changed oldfd's offset
changed oldfd's flags
old flags: 33793, new flags: 33793
old offset: 3, new offset: 3
 */

#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

int main() {
    int fd = open("x", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        errExit("open");
    }
    const char *w1 = "hello";
    int num_write = write(fd, w1, strlen(w1));
    if (num_write == -1) {
        errExit("write");
    }
    // now offset is at the end of file

    int old_flags = fcntl(fd, F_GETFL);
    if (old_flags == -1) {
        errExit("fcntl");
    }
    off_t old_offset = lseek(fd, 0, SEEK_CUR);

    int new_fd = dup(fd);   // create the duplicate
    int new_flags = fcntl(new_fd, F_GETFL);
    if (new_flags == -1) {
        errExit("fcntl");
    }
    off_t new_offset = lseek(new_fd, 0, SEEK_CUR);

    printf("old flags: %d, new flags: %d\n", old_flags, new_flags);
    printf("old offset: %lld, new offset: %lld\n", (long long)old_offset, (long long)new_offset);

    // now make some changes to the flags and the offset of the original fd
    if(lseek(fd, 3, SEEK_SET) == -1) {
        errExit("lseek");
    }
    printf("changed oldfd's offset\n");
    if(fcntl(fd, F_SETFL, old_flags | O_APPEND) == -1) {
        errExit("fcntl");
    }
    printf("changed oldfd's flags\n");

    old_flags = fcntl(fd, F_GETFL);
    if (old_flags == -1) {
        errExit("fcntl");
    }
    old_offset = lseek(fd, 0, SEEK_CUR);

    new_fd = dup(fd);   // create the duplicate
    new_flags = fcntl(new_fd, F_GETFL);
    if (new_flags == -1) {
        errExit("fcntl");
    }
    new_offset = lseek(new_fd, 0, SEEK_CUR);

    printf("old flags: %d, new flags: %d\n", old_flags, new_flags);
    printf("old offset: %lld, new offset: %lld\n", (long long)old_offset, (long long)new_offset);

    return 0;
}
