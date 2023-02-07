/* 系统调用 access()会利用进程的实际用户和组 ID 来检查权限。请编写相应函数，根
据进程的有效用户和组 ID 来进行权限检查。 */

/*
注意:access()检查的是real id,一般的系统调用都是与effective id相关.

可以使用setresuid()和setresgid()来交换real id和effective id:
(书中原文)非特权进程能够将实际用户 ID、有效用户 ID 和保存 set-user-ID 中的任一
ID 设置为实际 用户 ID、有效用户 ID 或保存 set-user-ID 之中的任一当前值。
特权级进程能够对其实际用户 ID、有效用户 ID 和保存 set-user-ID 做任意设置。

我使用的操作系统上有两个普通用户ubuntu(uid=1000)和lighthouse(uid=1001).lighthouse会被视为其他用户权限.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ ll
...
-------rwx  1 ubuntu ubuntu     6 Feb  6 16:05 tfile*
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ ./15.4 ./tfile        // effective id=1000,结果应该为---
init
user    real=1000 effective=1000 saved=1000
group   real=1000 effective=1000 saved=1000
swapped real and effective
user    real=1000 effective=1000 saved=1000
group   real=1000 effective=1000 saved=1000
perm for file './tfile' is ---
restored real and effective
user    real=1000 effective=1000 saved=1000
group   real=1000 effective=1000 saved=1000
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ su lighthouse
Password: 
__vsc_prompt_cmd_original: command not found
lighthouse@VM-4-12-ubuntu:/home/ubuntu/TLPI-note/15$ ./15.4 ./tfile     // effective id=1001,结果应该为rwx
init
user    real=1001 effective=1001 saved=1001
group   real=1001 effective=1001 saved=1001
swapped real and effective
user    real=1001 effective=1001 saved=1001
group   real=1001 effective=1001 saved=1001
perm for file './tfile' is rwx
restored real and effective
user    real=1001 effective=1001 saved=1001
group   real=1001 effective=1001 saved=1001
__vsc_prompt_cmd_original: command not found

要使real和effective不同,需要把程序设置为set-user-id程序.注意书上提到的一个细节,设置了s位的程序
在chown后会被重置为x,所以应该先chown再chmod.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ sudo chown lighthouse ./15.4
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ sudo chmod a+s ./15.4
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ ./15.4 ./tfile    // effective id=1000,结果应该为---
init
user    real=1000 effective=1001 saved=1001
group   real=1000 effective=1000 saved=1000
swapped real and effective
user    real=1001 effective=1000 saved=1001
group   real=1000 effective=1000 saved=1000
perm for file './tfile' is ---
restored real and effective
user    real=1000 effective=1001 saved=1001
group   real=1000 effective=1000 saved=1000

实际上,书上提供了这样一个答案= =
GNU C 函数库提供了以 euidaccess()命名的一个函数，请参考函数库源文件 sysdeps/
posix/euidaccess.c。

 */

#include <unistd.h>

#include "tlpi_hdr.h"

void show_ugid(const char *);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usageErr("%s file\n", argv[0]);
    }

    // get real id and effective id
    uid_t ruid, euid, suid;
    gid_t rgid, egid, sgid;

    if (getresuid(&ruid, &euid, &suid) == -1 ||
        getresgid(&rgid, &egid, &sgid) == -1)
        errExit("getresuid or getresgid");
    show_ugid("init");

    // swap real id and effective id
    if (setresuid(euid, ruid, -1) == -1 || setresgid(egid, rgid, -1) == -1)
        errExit("setresuid or setresgid");
    show_ugid("swapped real and effective");

    // call access()
    char mode[4];
    memset(mode, '-', 3);
    mode[3] = '\0';
    char *path = argv[1];
    if (access(path, F_OK) == -1) errExit("access: file not exist");
    if (!access(path, R_OK)) mode[0] = 'r';
    if (!access(path, W_OK)) mode[1] = 'w';
    if (!access(path, X_OK)) mode[2] = 'x';
    printf("perm for file '%s' is %s\n", path, mode);

    // restore
    if (setresuid(ruid, euid, -1) == -1 || setresgid(rgid, egid, -1) == -1)
        errExit("setresuid or setresgid");
    show_ugid("restored real and effective");

    return 0;
}

void show_ugid(const char *msg) {
    puts(msg);
    uid_t ruid, euid, suid;
    gid_t rgid, egid, sgid;

    if (getresuid(&ruid, &euid, &suid) == -1 ||
        getresgid(&rgid, &egid, &sgid) == -1)
        errExit("getresuid or getresgid");

    printf("user\treal=%d effective=%d saved=%d\n", ruid, euid, suid);
    printf("group\treal=%d effective=%d saved=%d\n", rgid, egid, sgid);
}
