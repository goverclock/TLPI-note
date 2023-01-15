/* 请使用标准文件 I/O 系统调用（open()和 lseek()）和 off_t 数据类型修改程序清单 5-3
中的程序。将宏_FILE_OFFSET_BITS 的值设置为 64 进行编译，并测试该程序是否
能够成功创建一个大文件。 */

/* 
原程序(与书中结果相同): 
$ ./5.1 x 10111222333
$ ls -l x
-rw------- 1 ubuntu ubuntu 10111222337 Jan 15 11:08 x
 
本程序与上述运行结果一致(除了时间部分).
另外发现即使不定义 _FILE_OFFSET_BITS 64 也能成功创建大文件.实际上这个宏在64位系统上是无效果的.
参见: https://stackoverflow.com/questions/48127585/when-writing-c-code-involving-files-should-i-define-file-offset-bits-64
(On 64 bit systems this macro has no effect since the *64 functions are identical to the normal functions.)
 */

// #define _FILE_OFFSET_BITS 64
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int fd;
    off_t off;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname offset\n", argv[0]);

    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("open");

    off = atoll(argv[2]);
    if (lseek(fd, off, SEEK_SET) == -1)
        errExit("lseek");

    if (write(fd, "test", 4) == -1)
        errExit("write");
    exit(EXIT_SUCCESS);
}
