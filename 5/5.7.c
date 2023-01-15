/* 使用 read()、write()以及 malloc 函数包（见 7.1.2 节）中的必要函数以实现 readv()
和 writev()功能。 */

/* 
#include <sys/uio.h>
ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
 
注意,因为readv和writev都是原子操作,所以用户内存和文件之间的数据传输必须一次性完成.
对于readv,这意味着需要一次性读取(read)所有缓冲区长度之合的数据量,再把数据分散到缓冲区中.
对于writev,这意味着需要把所有缓冲区中的数据拼接到一起,再一次性写入文件(write)
上面的过程中需要一个临时的区域存储数据,由malloc分配,free释放

这里有一个疑问,为了保证原子性只能调用一次read/write,这种情况下如果fd是一个套接字,终端之类的文件,就可能无法读完数据.
这种情况下返回值会低于iov的总长度,缓冲区里尚未发送的数据/填充的空间怎么处理呢?
 */

#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/uio.h>

ssize_t my_readv(int, const struct iovec*, int);
ssize_t my_writev(int, const struct iovec*, int);

// main() adopted from tlpi-dist/fileio/t_readv.c
int main(int argc, char *argv[]) {
    int fd;
    struct iovec iov[3];
    struct stat myStruct;       /* First buffer */
    int x;                      /* Second buffer */
#define STR_SIZE 100
    char str[STR_SIZE];         /* Third buffer */
    ssize_t numRead, totRequired;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);

    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
        errExit("open");

    totRequired = 0;

    iov[0].iov_base = &myStruct;
    iov[0].iov_len = sizeof(struct stat);
    totRequired += iov[0].iov_len;

    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(x);
    totRequired += iov[1].iov_len;

    iov[2].iov_base = str;
    iov[2].iov_len = STR_SIZE;
    totRequired += iov[2].iov_len;

    numRead = my_readv(fd, iov, 3);
    if (numRead == -1)
        errExit("my_readv");

    if (numRead < totRequired)
        printf("Read fewer bytes than requested\n");

    printf("total bytes requested: %ld; bytes read: %ld\n",
            (long) totRequired, (long) numRead);

    exit(EXIT_SUCCESS);

    return 0;
}

ssize_t my_readv(int fd, const struct iovec* iov, int iovcnt) {
    ssize_t tot_required = 0;
    for (int i = 0; i < iovcnt; i++) {
        tot_required += iov[i].iov_len;
    }
    char *buf = (char*)malloc(tot_required);

    // read only once
    ssize_t num_read = read(fd, buf, tot_required);
    if (num_read == -1) {
        free(buf);
        return -1;
    }

    // scatter buf to iovs
    ssize_t buf_ind = 0;
    for (int i = 0; i < iovcnt; i++) {
        void *base = iov[i].iov_base;
        size_t len = iov[i].iov_len;
        if (tot_required - buf_ind >= len) {
            memcpy(base, &buf[buf_ind], len);
            buf_ind += len;
        } else {
            memcpy(base, &buf[buf_ind], tot_required - buf_ind);
            break;
        }
    }

    free(buf);
    return num_read;
}

// not tested cause I'm lazy
ssize_t my_writev(int fd, const struct iovec* iov, int iovcnt) {
    ssize_t tot_reqired = 0;
    for (int i = 0; i < iovcnt; i++) {
        tot_reqired += iov[i].iov_len;
    }
    char *buf = (char*)malloc(tot_reqired);

    // gather data
    ssize_t buf_ind = 0;
    for (int i = 0; i < iovcnt; i++) {
        memcpy(&buf[buf_ind], iov[i].iov_base, iov[i].iov_len);
        buf_ind += iov[i].iov_len;
    }

    // write only once
    ssize_t num_write;
    if ((num_write = write(fd, buf, tot_reqired)) == -1) {
        free(buf);
        return -1;
    }
    free(buf);

    return num_write;
}
