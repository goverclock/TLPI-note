/*编写一个程序绘制树状结构，展示系统中所有进程的父子关系，根节点为 init 进程。
对每个进程而言，程序应该显示进程 ID 和所执行的行命令。程序输出类似于 pstree(1)
的输出结果，但也无需像后者那样复杂。每个进程的父进程可通过对/proc/PID/status
系统文件中 PPid：行的分析获得。但是需要小心处理如下可能性：在扫描所有
/proc/PID 目录的过程中，进程的父进程（以及父进程的/proc/PID 目录）消失了。*/

/*
思路是扫描所有/proc/PID/status,记录其中的Name,Pid,PPid字段,保存在一个数组中,接下来
就是建树过程了.

如果出现了题目中描述的情况,可能导致数组中某个进程的父进程不在数组中,我觉得这种进程忽略掉就好,在
建树算法里处理.
但这种情况导致另一个问题,即这个数组的大小应该是多少?扫描的过程中进程的数量是会变化的,无法预先确
,一个解决方案是使用realloc.

shell命令ps -e | wc
-l可以查看进程总数.注意到(在ubuntu上)pid为1的进程不是init,而是systemd
,实际上是一个符号链接.
*/

#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <malloc.h>
#include <unistd.h>

#include "tlpi_hdr.h"

#define MAX_NAME 1024
#define MAX_LINE 4096

struct procinfo {
    char name[MAX_NAME];
    pid_t pid;
    pid_t ppid;
};

int main() {
    DIR *dir_proc = opendir("/proc");
    if (dir_proc == NULL) {
        errExit("opendir");
    }

    struct dirent *cur_dir = NULL;
    int proc_cnt = 0;
    int proc_max = 1;
    struct procinfo *procs =
        (struct procinfo *)malloc(proc_max * sizeof(struct procinfo));
    if (procs == NULL) {
        errExit("malloc");
    }
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

        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "/proc/%s/status", cur_dir->d_name);
        FILE *fp = fopen(path, "r");
        if (fp == NULL) continue;

        bool gotName = false;
        bool gotPid = false;
        bool gotPPid = false;
        char line[MAX_LINE], cmd[MAX_LINE];
        pid_t Pid, PPid;
        while (!gotName || !gotPid || !gotPPid) {
            if (fgets(line, MAX_LINE, fp) == NULL) break;

            if (strncmp(line, "Name:", 5) == 0) {
                char *p;
                for (p = line + 5; *p != '\0' && isspace((unsigned char)*p);)
                    p++;
                strncpy(cmd, p, MAX_LINE - 1);
                cmd[MAX_LINE - 1] = '\0';

                gotName = true;
            }

            if (strncmp(line, "Pid:", 4) == 0) {
                Pid = strtol(line + 4, NULL, 10);
                gotPid = true;
            }

            if (strncmp(line, "PPid:", 5) == 0) {
                PPid = strtol(line + 5, NULL, 10);
                gotPPid = true;
            }
        }
        fclose(fp);

        strncpy(procs[proc_cnt].name, cmd, MAX_NAME - 1);
        procs[proc_cnt].name[MAX_NAME - 1] = '\0';
        procs[proc_cnt].pid = Pid;
        procs[proc_cnt].ppid = PPid;
        proc_cnt++;
        if (proc_cnt == proc_max) {
            proc_max *= 2;
            printf("%ld\n", proc_max * sizeof(struct procinfo));
            procs = (struct procinfo *)realloc(
                procs, proc_max * sizeof(struct procinfo));
            if (procs == NULL) {
                errExit("realloc");
            }
        }
    }

    for (int i = 0; i < proc_cnt; i++) {
        printf("p=%10d pp=%10d cmd=%s", procs[i].pid, procs[i].ppid, procs[i].name);
    }
    printf("tot procs found:%d\n", proc_cnt);

    // build tree
    // I'm too lazy to do it

    return 0;
}
