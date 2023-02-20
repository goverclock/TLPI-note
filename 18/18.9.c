/* 18.10 节展示了两种技术（分别为 fchdir()和 chdir()），用于在将当前工作目录转到
另一位置后，再返回之前的当前工作目录。假设需要反复执行这一操作，哪种方法
更为高效？原因何在？请写一段程序加以验证。 */

/* 
书中提供的答案:

使用 fchdir()调用更为高效。如果在循环中反复执行操作，那么当调用 fchdir()时，
可以在运行循环前调用一次 open()；而当调用 chdir()时，可以将 getcwd()调用置于
循环之外。随后可以比较重复调用 fchdir(fd) 和 chdir(buf)之间的差异。调用 chdir()
之所以代价高昂，有两点原因：传递 buf 参数到内核需要在用户空间和内核空间之
间进行大数据量传输，每次调用时必须将 buf 中的路径名解析到相应目录的 i-node
上。（内核对目录条目信息的高速缓存减少了第二个原因的开销，但总有些工作是
省不了的。）
 
可以看到两者用时差了几乎一倍,chdir的时间确实主要消耗在了sys上.
 
ubuntu@VM-4-12-ubuntu:~/TLPI-note/18$ time ./18.9

real    0m4.607s
user    0m2.432s
sys     0m2.172s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/18$ time ./18.9

real    0m4.627s
user    0m2.571s
sys     0m2.055s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/18$ time ./18.9 x

real    0m8.190s
user    0m2.361s
sys     0m5.828s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/18$ time ./18.9 x

real    0m8.364s
user    0m2.481s
sys     0m5.880s
 */

#include "tlpi_hdr.h"
#include <linux/limits.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int fd = open(".", O_RDONLY);
    if (fd == -1) errExit("open");

    char path[PATH_MAX];
    if (getcwd(path, PATH_MAX) == NULL) errExit("getcwd");

    int n = 10000000;
    if (argc == 1) {    // fchdir
        while(n--) fchdir(fd);
    } else {
        while(n--) chdir(path);
    }

    return 0;
}
