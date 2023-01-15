/* 使用 fcntl()和 close()（若有必要）来实现 dup()和 dup2()。（对于某些错误，dup2()
和 fcntl()返回的 errno 值并不相同，此处可不予考虑。）务必牢记 dup2()需要处理的
一种特殊情况，即 oldfd 与 newfd 相等。这时，应检查 oldfd 是否有效，测试 fcntl
(oldfd，F_GETFL)是否成功就能达到这一目的。若 oldfd 无效，则 dup2()将返回-1，
并将 errno 置为 EBADF。 */

/*  
int dup(int oldfd) - 复制描述符oldfd,返回newfd,两个描述符都指向同一个打开文件句柄
int dup2(int oldfd, int newfd) - 创建描述符oldfd的副本,其数值由newfd参数指定

主要用到fcntl(oldfd, F_DUPFD, startfd)来复制描述符.

*/

#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <fcntl.h>

int fcn_dup(int);
int fcn_dup2(int, int);

int main() {
    
}

int fcn_dup(int oldfd) {
    return fcntl(oldfd, F_DUPFD, 0);
}

int fcn_dup2(int oldfd, int newfd) {
    // 1. 如果oldfd无效,失败并返回错误EBADF
    int flags = fcntl(oldfd, F_GETFL);
    if (flags == -1) {
        errno = EBADF;
        return -1;
    }

    // 2. 如果oldfd有效,且与newfd值相等,则什么都不做
    if (oldfd == newfd) {
        return oldfd;
    }

    // 3. 否则关闭newfd(忽视可能出现的错误),创建副本
    return fcntl(oldfd, F_DUPFD, newfd);
}
