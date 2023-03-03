/* 如果子进程调用 exit(-1)，父进程将会看到何种退出状态（由 WEXITSTATUS()返回）？
 */

/* 
输出是255.
还是要用WEXITSTATUS的,不知道为什么勘误表把它删掉了.
 */

#include "tlpi_hdr.h"
#include <sys/wait.h>

int main() {
    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:  // child
            exit(-1);
        default:
            break;
    }
    // parent
    int status;
    if (wait(&status) == -1) errExit("wait");
    printf("%d\n", WEXITSTATUS(status));

    exit(EXIT_SUCCESS);
}
