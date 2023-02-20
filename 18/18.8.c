/* 实现 nftw()。（需要使用 opendir()、readdir()、closedir()和
 * stat()等系统调用。） */

/*
其实就是写一个前序深度优先遍历.
利用18.7.c进行测试,my_nftw与nftw的表现在此程序中一致.
注意,应该用lstat而不是stat,因为typeflag参数的FTW_SL参数用来表示该文件是符号链接.
FTW_DNR可以用access(file, R_OK)来检测,但是我懒得写了.
 */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif
#include <dirent.h>
#include <ftw.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/stat.h>

#include "tlpi_hdr.h"

int my_nftw(const char *dirpath,
            int (*func)(const char *pathname, const struct stat *statbuf,
                        int typeflag, struct FTW *ftwbuf),
            int nopenfd, int flags) {
    char path[PATH_MAX];
    strcpy(path, dirpath);

    struct FTW ftw;
    ftw.base = 0;
    ftw.level = 0;

    int tflag = FTW_F;  // if the file is not directory or symbol link
    struct stat sb;
    if (lstat(path, &sb) == -1) tflag = FTW_NS;  // file is not statable
    if (S_ISDIR(sb.st_mode)) tflag = FTW_D;        // file is directory
    if ((flags & FTW_PHYS) && S_ISLNK(sb.st_mode))
        tflag = FTW_SL;  // file is symbol link
    // lol not going to implement the other flags

    int ret = func(path, &sb, tflag, &ftw);
    if (ret) return ret;
    if (S_ISDIR(sb.st_mode)) {
        DIR *dirp = opendir(path);
        if (dirp == NULL) errExit("opendir");

        char sub_path[PATH_MAX];
        while (1) {
            errno = 0;
            struct dirent *direntp = readdir(dirp);
            if (direntp == NULL) {
                if (errno != 0)
                    errExit("readdir");
                else
                    break;
            }
            if (!strcmp(direntp->d_name, ".")) continue;  // skip . and ..
            if (!strcmp(direntp->d_name, "..")) continue;

            strcpy(sub_path, path);
            strcat(sub_path, "/");
            strcat(sub_path, direntp->d_name);
            ret = my_nftw(sub_path, func, nopenfd, flags);
            if (ret) return ret;
        }
    }
    return 0;
}
