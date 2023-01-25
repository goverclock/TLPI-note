/* 使用 setgroups()及库函数从密码文件、组文件（参见 8.4 节）中获取信息，以实现
initgroups()。请注意，欲调用 setgroups()，进程必须享有特权。
real=0 effective=1000 saved=1000 file-system=1000 */

/*
ubuntu默认的用户UID为1000,不具有特权.**当创建新进程时,实际(real)用户ID会继承其父进程**,也就是UID=1000
的用户的终端,使得程序不具有特权.(real=effective=saved=1000)
所以需要把这个程序变成set-user-ID程序,方法如下:
$ su
Password:
# chown root ./9.3      使得程序归root用户所属
# chmod u+s ./9.3       使得程序运行时将有效用户设置为其属主(即root)

注意,此时运行此程序时的实际(real)用户ID仍然为创建此进程的终端的用户ID.
当执行程序时,saved id由effective id复制而来.
$ sudo ./9.3
real=0 effective=0 saved=0
...
$ ./9.3
real=1000 effective=0 saved=0
...

以上是背景知识,与题目的要求不符合,要符合real=0 effective=1000 saved=1000
file-system=1000, 意味着文件属主应该为UID=1000,chmod
u+s,然后作为root执行程序.在程序运行过程中要获取特权,则需要将 有效ID修改为real
ID.

setgroups()设置辅助组集合,initgroup()从/etc/group文件中读取并设置一个用户的辅助组集合.

/etc/group的形式如下:
$ cat /etc/group
root:x:0:
daemon:x:1:
bin:x:2:
sys:x:3:
adm:x:4:syslog,ubuntu
tty:x:5:syslog
disk:x:6:
lp:x:7:
...

运行结果:
$ sudo ./9.3
real=0 effective=1000 saved=1000
groups(before)
0
groups(after)
4
24
27
30
46
116
1000

附:
$ cat /etc/group | 
grep ubuntu
adm:x:4:syslog,ubuntu
cdrom:x:24:ubuntu
sudo:x:27:ubuntu
dip:x:30:ubuntu
plugdev:x:46:ubuntu
lxd:x:116:ubuntu
ubuntu:x:1000:
 */

#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <unistd.h>

#include "tlpi_hdr.h"

int my_initgroups(const char *, gid_t);
void show_group(const char *);

int main() {
    uid_t real, effe, save;
    if (getresuid(&real, &effe, &save) == -1) {
        errExit("getresuid");
    }
    printf("real=%d effective=%d saved=%d\n", real, effe, save);
    if (real != 0) {
        puts("should run as root");
        exit(-1);
    }
    if (effe != 1000) {
        puts("shold run as real=0 effective=1000 saved=1000\n");
        exit(-1);
    }
    show_group("groups(before)");

    struct passwd *usr =
        getpwuid(1000);  // initgroup()函数的主要用户一般通过读取密码文件
                         // 中用户记录的组属性来获取参数group的值
    if (usr == NULL) {
        errExit("getpwuid");
    }
    if (my_initgroups(usr->pw_name, usr->pw_gid) == -1) {
        errExit("my_initgroups");
    }

    show_group("groups(after)");
    return 0;
}

int my_initgroups(const char *user, gid_t group) {
    gid_t grps[NGROUPS_MAX + 1];
    int gidsize = 0;

    struct group
        *grp;  // 扫描/etc/group可以用第八章介绍的几个函数,无需手动处理文件
    while ((grp = getgrent()) != NULL) {
        char **grmem = grp->gr_mem;
        if (grmem == NULL) {
            return -1;
        }
        for (int i = 0; grmem[i] != NULL; i++)
            if (strcmp(user, grmem[i]) == 0) grps[gidsize++] = grp->gr_gid;
    }
    endgrent();
    grps[gidsize++] = group;
    uid_t effe = geteuid();
    // 获取特权,0是real id
    seteuid(0);
    int ret = setgroups(gidsize, grps); // setgroups需要特权
    seteuid(effe);  // 恢复原权限
    return ret;
}

void show_group(const char *msg) {
    puts(msg);
    gid_t grps[NGROUPS_MAX + 1];
    int gidsize = getgroups(NGROUPS_MAX + 1, grps);
    if (gidsize == -1) {
        errExit("getgroups");
    }
    for (int i = 0; i < gidsize; i++) {
        printf("%d\n", grps[i]);
    }
}
