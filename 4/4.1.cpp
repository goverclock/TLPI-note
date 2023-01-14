/* tee 命令是从标准输入中读取数据，直至文件结尾，随后将数据写入标准输出和命令行
参数所指定的文件。（44.7 节讨论 FIFO 时，会展示使用 tee 命令的一个例子。）请使用
I/O 系统调用实现 tee 命令。默认情况下，若已存在与命令行参数指定文件同名的文件，
tee 命令会将其覆盖。如文件已存在，请实现-a 命令行选项（tee -a file）在文件结尾处
追加数据。（请参考附录 B 中对 getopt()函数的描述来解析命令行选项。 */

#include "tlpi_hdr.h"
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc > 3 || (argc > 1 && strcmp(argv[1], "--help") == 0)) {
        usageErr("%s [[-a] file]\n", argv[0]);
    }
    
    int fd = -1;
    
    int opt;
    while((opt = getopt(argc, argv, ":a:")) != -1) {
        if (opt == '?') {
            errExit("unrecognized command line option '%c'\n", optopt);
        } else if (opt == ':') {
            errExit("missing arguments for '-%c'\n", optopt);
        } else {    // opt == 'a'
            fd = open(optarg, O_WRONLY | O_CREAT | O_APPEND);
            if (fd == -1) {
                errExit("open");
            }
        }
    }
    if (fd == -1 && argc > 1) {
        fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC);
        if (fd == -1) {
            errExit("open");
        }
    }

    char buf[BUF_SIZE];
    int num_read = 0;
    while((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        write(STDOUT_FILENO, buf, num_read);
        if (fd != -1) {
            write(fd, buf, num_read);
        }
    }

    return 0;
}