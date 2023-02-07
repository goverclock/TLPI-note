/* 命令 chmod a+rX file 的作用是对所有各类用户授予读权限，并且，当 file 是目录，
或者 file 的任一用户类型具有可执行权限时，将向所有各类用户授予可执行权限.

使用 stat()和 chmod()编写一程序，令其等效于执行 chmod a+rX 命令。 */

/* 
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ ./t.sh 
dr-------- 2 ubuntu ubuntu 4096 Feb  7 10:06 dir
-r-------- 1 ubuntu ubuntu    0 Feb  7 10:06 file
-r-x------ 1 ubuntu ubuntu    0 Feb  7 10:06 prog
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ ./15.6 dir file prog
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ ls -ld dir file prog
dr-xr-xr-x 2 ubuntu ubuntu 4096 Feb  7 10:06 dir
-r--r--r-- 1 ubuntu ubuntu    0 Feb  7 10:06 file
-r-xr-xr-x 1 ubuntu ubuntu    0 Feb  7 10:06 prog
 */

#include <sys/stat.h>

#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    if (argc < 2) usageErr("%s file1 file2 file3 ...", argv[0]);

    for (int i = 1; i < argc; i++) {
        struct stat sb;
        mode_t mode;
        if (stat(argv[i], &sb) == -1) errExit("stat");
        mode = sb.st_mode;

        mode |= S_IRUSR | S_IRGRP | S_IROTH;  // give read permission to all
        if (S_ISDIR(mode) || (mode & S_IXUSR) || (mode & S_IXGRP) || (mode & S_IXOTH))
            mode |= S_IXUSR | S_IXGRP | S_IXOTH;

        if (chmod(argv[i], mode) == -1) errExit("chmod");
    }

    return 0;
}
