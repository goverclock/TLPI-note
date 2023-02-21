/* 编写一个程序，针对其命令行参数所指定的目录，记录所有的文件创建、删除和改
名操作。该程序应能够监控指定目录下所有子目录中的事件。获得所有子目录的列
表需使用 nftw()（参见 18.9 节）。当在目录树下添加或删除了子目录时，受监控的
子目录集合应能保持同步更新。 */

/*
注意,要监控一个目录中所有的文件创建,删除和改名操作,只需要监控这个目录就可以了,不需要监控
目录中的所有文件:
inotify(7)
When a directory is monitored, inotify will return events for the directory
itself, and for files(我注:不包含子目录中的文件) inside the directory.

但是如果还要监控目录中子目录中的变化,仍然需要单独监控这个子目录.朴素的做法是直接对原目录重新调用一次
nftw(),因为inotify_add_watch()对同一inotify fd和路径名的重复调用只是替换了mask.
当受监控对象被删除(或其所驻留的文件系统遭卸载)时,内核会隐式删除监控项,所以这一点不需要手动维护.

注意书上提到的这个细节:

当受监控目录中有文件发生事件时，name 字段返回一个以空字符结尾的字符串，以标识
该文件。若受监控对象自身有事件发生，则不使用 name 字段，将 len 字段置 0。
 */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif
#include <ftw.h>
#include <linux/limits.h>
#include <sys/inotify.h>

#include "tlpi_hdr.h"

#define BUF_LEN (sizeof(struct inotify_event) + NAME_MAX + 1)

static int ifd = -1;

void watch_tree(int ifd, char *rt);
int watch_file(const char *pathname, const struct stat *statbuf, int typeflag,
               struct FTW *ftwbuf);
void display_event(struct inotify_event *eventp);

int main(int argc, char *argv[]) {
    if (argc < 2) usageErr("%s directories...\n", argv[0]);

    ifd = inotify_init();
    if (ifd == -1) errExit("inotify_init");
    for (int i = 1; i < argc; i++) {
        struct stat fi;
        if (stat(argv[i], &fi) == -1) errExit("stat argv[i]");
        if (!S_ISDIR(fi.st_mode)) usageErr("%s is not a directory\n", argv[i]);
        watch_tree(ifd, argv[i]);
    }

    char buf[BUF_LEN] __attribute__((aligned(8)));
    ;
    while (1) {
        int num_read = read(ifd, buf, BUF_LEN);
        if (num_read == -1) errExit("read");
        if (num_read == 0) errExit("the fuck this is impossible");

        printf("*read %ld bytes from inotify fd\n", (long)num_read);
        char *p = buf;
        while (p < buf + num_read) {
            struct inotify_event *eventp = (struct inotify_event *)p;
            display_event(eventp);
            // if the event indicates that a new directory was created
            // then watch it
            if (eventp->mask & IN_CREATE)
                for (int i = 1; i < argc; i++) {    // brutal, we are evil
                    struct stat fi;
                    if (stat(argv[i], &fi) == -1) errExit("stat argv[i]");
                    if (!S_ISDIR(fi.st_mode))
                        usageErr("%s is not a directory\n", argv[i]);
                    watch_tree(ifd, argv[i]);
                }
            p += sizeof(struct inotify_event) + eventp->len;
        }
    }

    return 0;
}

void watch_tree(int ifd, char *rt) {
    if (nftw(rt, watch_file, 20, FTW_PHYS) != 0) errExit("nftw");
}

int watch_file(const char *pathname, const struct stat *sb, int flag,
               struct FTW *fb) {
    if (flag == FTW_NS) errExit("not statable? wtf");
    if (!S_ISDIR(sb->st_mode)) return 0;  // only watch directory

    int wd = inotify_add_watch(ifd, pathname, IN_ALL_EVENTS);
    if (wd == -1) errExit("inotify_add_watch");
    printf("watching %s with wd=%d\n", pathname, wd);
    return 0;
}

void display_event(struct inotify_event *eventp) {
    puts("<---   ");
    printf("wd=%d ", eventp->wd);

    if (eventp->mask & IN_ACCESS) printf("IN_ACCESS ");
    if (eventp->mask & IN_ATTRIB) printf("IN_ATTRIB ");
    if (eventp->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (eventp->mask & IN_CLOSE_WRITE) printf("IN_CLOSE_WRITE ");
    if (eventp->mask & IN_CREATE) printf("IN_CREATE ");
    if (eventp->mask & IN_DELETE) printf("IN_DELETE ");
    if (eventp->mask & IN_DELETE_SELF) printf("IN_DELETE_SELF ");
    if (eventp->mask & IN_IGNORED) printf("IN_IGNORED ");
    if (eventp->mask & IN_ISDIR) printf("IN_ISDIR ");
    if (eventp->mask & IN_MODIFY) printf("IN_MODIFY ");
    if (eventp->mask & IN_MOVE_SELF) printf("IN_MOVE_SELF ");
    if (eventp->mask & IN_MOVED_FROM) printf("IN_MOVED_FROM ");
    if (eventp->mask & IN_MOVED_TO) printf("IN_MOVED_TO ");
    if (eventp->mask & IN_OPEN) printf("IN_OPEN ");
    if (eventp->mask & IN_Q_OVERFLOW) printf("IN_Q_OVERFLOW ");
    if (eventp->mask & IN_UNMOUNT) printf("IN_UNMOUNT ");
    printf("\n");

    if (eventp->cookie != 0) printf("cookie=%d\n", eventp->cookie);
    if (eventp->len > 0) printf("name=%s\n", eventp->name);
    puts("   --->");
}
