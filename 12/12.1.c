/* 编写一个程序，以用户名作为命令行参数，列表显示该用户下所有正在运行的进程
ID 和命令名。（程序清单 8-1 中的 userIdFromName()函数对本题程序的编写可能会
有所帮助。）通过分析系统中/proc/PID/status 文件的 Name：和 Uid：各行信息，可
以实现此功能。遍历系统的所有/proc/PID 目录需要使用 readdir(3)函数，18.8 节对
其进行了描述。程序必须能够正确处理如下可能性：在确定目录存在与程序尝试打
开相应/proc/PID/status 文件之间，/proc/PID 目录消失了。 */

/*
对于"在确定目录存在与程序尝试打开相应/proc/PID/status
文件之间，/proc/PID目录消失了"这一情况,
open调用会失败,只需要照常检查open的返回值就可以应对.
根据test.c中进行的测试,不需要特殊处理open成功后文件消失的情况.
 */

#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "tlpi_hdr.h"

#define MAX_LINE 1024

// from 8-1
uid_t userIdFromName(const char *name);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usageErr("%s username\n", argv[0]);
        exit(-1);
    }

    uid_t userid = userIdFromName(argv[1]);
    if (userid == -1) {
        errExit("userIdFromName");
    }
    DIR *dir_proc = opendir("/proc");
    if (dir_proc == NULL) {
        errExit("opendir");
    }

    struct dirent *cur_dir = NULL;
    while (1) {
        errno = 0;
        if ((cur_dir = readdir(dir_proc)) == NULL) {
            if (errno == 0)  // end of directory
                break;
            else
                errExit("readdir");
        }

        if (cur_dir->d_type != DT_DIR ||
            !isdigit((unsigned char)cur_dir->d_name[0]))
            continue;

        // copy of procfs_user_exe.c
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "/proc/%s/status", cur_dir->d_name);
        FILE *fp = fopen(path, "r");
        if (fp == NULL) continue;

        bool gotName = false;
        bool gotUid = false;
        char line[MAX_LINE], cmd[MAX_LINE];
        uid_t uid;
        while (!gotName || !gotUid) {
            if (fgets(line, MAX_LINE, fp) == NULL) break;

            if (strncmp(line, "Name:", 5) == 0) {
                char *p;
                for (p = line + 5; *p != '\0' && isspace((unsigned char)*p);)
                    p++;
                strncpy(cmd, p, MAX_LINE - 1);
                cmd[MAX_LINE - 1] = '\0';

                gotName = true;
            }

            if (strncmp(line, "Uid:", 4) == 0) {
                uid = strtol(line + 4, NULL, 10);
                gotUid = true;
            }
        }

        fclose(fp);

        if (gotName && gotUid && uid == userid)
            printf("%5s %s", cur_dir->d_name, cmd);
    }

    return 0;
}

uid_t /* Return UID corresponding to 'name', or -1 on error */
userIdFromName(const char *name) {
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    if (name == NULL || *name == '\0') /* On NULL or empty string */
        return -1;                     /* return an error */

    u = strtol(name, &endptr, 10); /* As a convenience to caller */
    if (*endptr == '\0')           /* allow a numeric string */
        return u;

    pwd = getpwnam(name);
    if (pwd == NULL) return -1;

    return pwd->pw_uid;
}