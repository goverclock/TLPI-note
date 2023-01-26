/* 针对执行 set-user-ID-root 程序的进程，重复上述练习，进程凭证的初始值如下
    real=X effective=0 saved=0 */

/* 
set-user-ID-root指root用户所拥有的set-user-ID程序.
 */

#include "tlpi_hdr.h"

void show_uid(const char *);

int main() {
    show_uid("init");

    uid_t ruid, euid, suid;
    if (getresuid(&ruid, &euid, &suid) == -1) {
        errExit("getresuid");
    }
    if (ruid == 0) {
        puts("should run as non-root");
        exit(-1);
    }

    // a) change effective id to real id
    if(seteuid(ruid) == -1) {
        errExit("seteuid");
    }
    show_uid("changed effective id to real id");

    // a) restore effective id to saved id
    if (seteuid(suid) == -1) {
        errExit("setuid");
    }
    show_uid("restored effective id to saved id");

    // b) give up set-user-ID
    if (setreuid(getuid(), getuid()) == -1) {   // see header comment
        errExit("setuid");
    }
    show_uid("give up set-user-ID");

    return 0;
}

void show_uid(const char *msg) {
    puts(msg);
    uid_t ruid, euid, suid;
    if (getresuid(&ruid, &euid, &suid) == -1) {
        errExit("getresuid");
    }
    printf("real=%d effective=%d saved=%d\n", ruid, euid, suid);
}
