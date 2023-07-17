/* 编写一个程序来验证当位于孤儿进程组中的一个进程试图从控制终端调用 read()
时会得到 EIO 的错误。 */

/* 
$ ./a.out 
$ Input/output error
 */

#include "tlpi_hdr.h"
#include <unistd.h>

int main() {
    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:     // child
            sleep(3);
            char buf[2];
            if (read(STDIN_FILENO, buf, 2) == -1) {
                puts(strerror(errno));
            } 
            break;
        default:    // parent
            // fall through and exit
            ;
    }

    exit(EXIT_SUCCESS);
}
