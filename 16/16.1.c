/* 编写一程序，可创建或修改文件的 user EA（亦即，setfattr(1)的简化版）。应将文件
名、EA 名以及 EA 值以命令行参数形式提供给该程序。 */

/* 
我比较懒.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/16$ getfattr -d ./tfile 
# file: tfile
user.x="hahaha"
user.y="good"

ubuntu@VM-4-12-ubuntu:~/TLPI-note/16$ ./16.1 tfile user.y bad
ubuntu@VM-4-12-ubuntu:~/TLPI-note/16$ getfattr -d ./tfile 
# file: tfile
user.x="hahaha"
user.y="bad"
 */

#include "tlpi_hdr.h"
#include <sys/xattr.h>

int main(int argc, char *argv[]) {
    if (argc != 4) usageErr("%s file name value", argv[0]);

    char *file = argv[1];
    char *name = argv[2];
    char *value = argv[3];
    if (setxattr(file, name, value, strlen(value), 0) == -1)
        errExit("setxattr");

    return 0;
}
