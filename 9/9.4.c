/* 假设某进程的所有用户标识均为 X，执行了用户 ID 为 Y 的 set-user-ID 程序，且 Y
为非 0 值，对进程凭证的设置如下：
real=X effective=Y saved=Y
（这里忽略了文件系统用户 ID，因为该 ID 随有效用户 ID 的变化而变化。）为
执行如下操作，请分别列出对 setuid()、seteuid()、setreuid()和 setresuid()的调用。
a） 挂起和恢复 set-user-ID 身份（即将有效用户 ID 在实际用户 ID 和保存
set-user-ID 间切换）。
b） 永久放弃 set-user-ID 身份（即确保将有效用户 ID 和保存 set-user-ID 设置
为实际用户 ID）。
（该练习还需要使用getuid()和geteuid()函数来获取进程的实际用户ID和有效用
户 ID。）请注意，鉴于上述列出的某些系统调用，部分操作将无法实现。 */

/* 
注意setreuid有一个行为:只要ruid参数不为-1,就把saved ID设置成新的有效用户ID.
这类调用很多都有类似的隐含行为,使用时参照资料.
 */

#include "tlpi_hdr.h"

void show_uid(const char *);

int main() {
    show_uid("init");

    uid_t ruid, euid, suid;
    if (getresuid(&ruid, &euid, &suid) == -1) {
        errExit("getresuid");
    }
    if (ruid != 0) {
        puts("shold run as root");
        exit(-1);
    }

    // a) change effective id to real id
    if (setuid(ruid) == -1) {
        errExit("setuid");
    }
    show_uid("changed effective id to real id");

    // a) restore effective id to saved id
    if (seteuid(suid) == -1) {  // use seteuid() rather than setuid()
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
