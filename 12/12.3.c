/* 编写一个程序，列表展示打开同一特定路径名文件的所有进程。可以通过分析所有
/proc/PID/fd/*符号链接的内容来实现此功能。这需要利用 readdir(3)函数来嵌套循
环，扫描所有/proc/PID 目录以及每个/proc/PID 目录下所有/proc/PID/fd 的条目内容。
读取/proc/PID/fd/n 符号链接的内容，需要使用 readlink()，18.5 节对其进行了描述。 */

/* 
读取/proc/PID/fd目录需要特权.
使用sudo ./12.3 path运行,例如sudo ./12.3 /dev/null
 */

#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usageErr("%s path\n", argv[0]);
        exit(-1);
    }

    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        errExit("opendir");
    }

    // scan /proc/PID
    struct dirent *cur_ent = NULL;
    while (1) {
        errno = 0;
        if((cur_ent = readdir(proc_dir)) == NULL) {
            if(errno == 0)
                break;
            else
                errExit("readdir");
        }

        if(cur_ent->d_type != DT_DIR || !isdigit((unsigned char)cur_ent->d_name[0]))
            continue;
        
        char fddirpath[PATH_MAX];
        snprintf(fddirpath, PATH_MAX, "/proc/%s/fd", cur_ent->d_name);
        // puts(fddirpath);
        DIR *fd_dir = opendir(fddirpath);
        if (fd_dir == NULL) {
            errExit("opendir");
        }
        // scan /proc/PID/fd
        struct dirent *fd_ent = NULL;
        while(1) {
            errno = 0;
            if((fd_ent = readdir(fd_dir)) == NULL) {
                if(errno == 0)
                    break;
                else
                    errExit("readdir");
            }
            if (!isdigit((unsigned char)fd_ent->d_name[0]))
                continue;

            char buf[PATH_MAX];
            char fdpath[PATH_MAX];
            snprintf(fdpath, PATH_MAX, "/proc/%s/fd/%s", cur_ent->d_name, fd_ent->d_name);
            // puts(fdpath);
            if (readlink(fdpath, buf, PATH_MAX) == -1) {
                errExit("readlink");
            }
            // puts(buf);
            if (strcmp(argv[1], buf) == 0) {
                printf("pid=%s fd=%s\n", cur_ent->d_name, fd_ent->d_name);
                break;
            }
        }
    }

    return 0;
}
