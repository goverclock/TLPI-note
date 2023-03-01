/* 编写一个程序以便验证调用 vfork()之后，子进程可以关闭一文件描述符（例如：描
述符 0）而不影响对应父进程中的文件描述符。 */

#include "tlpi_hdr.h"
#include <fcntl.h>

int main() {
    switch (vfork()) {
        case -1:
            errExit("vfork");
        case 0:  // child
            if (close(STDIN_FILENO) == -1) errExit("close child");
            _exit(EXIT_SUCCESS);
        default:  // parent
            if (fcntl(STDIN_FILENO, F_GETFD) == -1 && errno == EBADF)
                puts("FUCK");
            else
                puts("GOOD");
    }

    exit(EXIT_SUCCESS);
}
