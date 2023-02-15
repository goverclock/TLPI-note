/* 实现 realpath()。 */

/*
不需要手动实现对符号链接的解析.
方法是先把进程的工作目录切换到待解析路径,然后调用getcwd().
注意判断目标路径是文件还是目录.

basename()和dirname()的行为看起来就想是返回了指向其参数指向的内存区域中的一块,例如当参数为
0x5630a8a242b0(指向"../README.md")时,basename()的返回值是0x5630a8a242b3,dirname()
的返回值等于参数.
 */

#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "tlpi_hdr.h"

char *my_realpath(const char *, char *);

int main(int argc, char *argv[]) {
    puts(my_realpath(argv[1], NULL));

    return 0;
}

char *my_realpath(const char *path, char *resolved_path) {
    if (resolved_path == NULL) {
        resolved_path = (char *)malloc(PATH_MAX);
        if (resolved_path == NULL) errExit("malloc");
    }

    // is path a file or directory?
    struct stat fi;
    char *base = NULL;
    char *t1 = NULL, *t2 = NULL;
    if (stat(path, &fi) == -1) errExit("stat");
    if (!S_ISDIR(fi.st_mode)) {
        t1 = strdup(path);
        t2 = strdup(path);
        if (t1 == NULL || t2 == NULL) errExit("strdup");
        base = basename(t1);
        path = dirname(t2);
    }

    // save cwd
    int fd = open(".", O_RDONLY);
    if (fd == -1) errExit("open");

    if (chdir(path) == -1) errExit("chdir");
    if (getcwd(resolved_path, PATH_MAX) == NULL) errExit("getcwd");
    fchdir(fd);  // restore cwd
    close(fd);

    if (base != NULL) {
        int l = strlen(resolved_path);  // no base name
        strcpy(resolved_path + l + 1, base);
        resolved_path[l] = '/';
    }
    if (t1 != NULL) free(t1);
    if (t2 != NULL) free(t2);

    return resolved_path;
}
