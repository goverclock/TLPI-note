/* 实现一个功能与 getcwd()相当的函数。提示：要获得当前工作目录的名称，可调用
opendir()和 readdir()来遍历其父目录（..）中的各个条目，查找其中与当前工作目录
具有相同 i-node 编号及设备编号（即，分别为 stat()和 lstat()调用所返回 stat
结构中 的 st_ino 和 st_dev
属性）的一项。如此这般，沿着目录树层层拾级而上（chdir("..")）
并进行扫描，就能构建出完整的目录路径。当父目录与当前工作目录相同时（回忆
/..与/相同的情况），就结束遍历。无论调用该函数成功与否，都应将调用者遣回其
起始目录（使用 open()和 fchdir()能很方便地实现这一功能）。 */

#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tlpi_hdr.h"

/*
stat结构不包含名称这一项,所以这件事情就变得如此复杂.
 */

char *my_getcwd(char *buf, size_t size);
char *cwd_name(char *buf, size_t size);  // return cwd's directory name

int main() {
    char buf[PATH_MAX];
    my_getcwd(buf, PATH_MAX);
    puts(buf);

    return 0;
}

char *my_getcwd(char *buf, size_t size) {
    // get cwd fd for future restore
    int fd = open(".", O_RDONLY);
    if (fd == -1) errExit("open .");

    int ind = 0;
    char cur_name[PATH_MAX];
    char tmp[PATH_MAX];
    tmp[0] = '\0';
    buf[0] = '\0';
    while (1) {
        if (cwd_name(cur_name, PATH_MAX) == NULL) errExit("cwd_name");
        sprintf(buf, "%s/%s", cur_name, tmp);
        strcpy(tmp, buf);
        if (!strcmp(cur_name, "/")) break;  // reached root
        if (chdir("..") == -1) errExit("chdir");
    }
    // chop leading '/' and trailing '/'
    strcpy(buf, buf + 1);
    buf[strlen(buf) - 1] = '\0';

    // restore working directory
    if (fchdir(fd) == -1) errExit("fchdir");
    return buf;
}

// return NULL on error
char *cwd_name(char *buf, size_t size) {
    // get cwd st_ino and st_dev
    struct stat cur_fi;
    if (stat(".", &cur_fi) == -1) errExit("stat .");

    // if parent == cwd, return "/"
    struct stat par_fi;
    if (stat("..", &par_fi) == -1) errExit("stat ..");
    if (cur_fi.st_ino == par_fi.st_ino && cur_fi.st_dev == par_fi.st_dev)
        return strcpy(buf, "/");

    // open and scan parent dir
    DIR *dirp = opendir("..");
    if (dirp == NULL) errExit("opendir ..");

    struct dirent *direntp = NULL;
    while (1) {
        errno = 0;
        direntp = readdir(dirp);
        if (direntp == NULL) {
            if (errno != 0)
                errExit("readdir");
            else
                break;
        }
        struct stat fi;
        char path[PATH_MAX] = {'.', '.', '/', '\0'};
        strcat(path + 3, direntp->d_name);
        if (stat(path, &fi) == -1) errExit("stat ..");

        // found cur dir
        if (fi.st_ino == cur_fi.st_ino && fi.st_dev == cur_fi.st_dev) {
            if (strlen(direntp->d_name) >= size) return NULL;
            return strcpy(buf, direntp->d_name);
        }
    }

    return NULL;
}
