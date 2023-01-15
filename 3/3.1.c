/* 
 * 使用 man 2 reboot 查询到相关信息:

 * This system call fails (with the error EINVAL)
       unless  magic equals LINUX_REBOOT_MAGIC1 (that
       is, 0xfee1dead) and  magic2  equals  LINUX_RE‐
       BOOT_MAGIC2  (that  is,  672274793).  However,
       since 2.1.17 also  LINUX_REBOOT_MAGIC2A  (that
       is,  85072278) and since 2.1.97 also LINUX_RE‐
       BOOT_MAGIC2B (that is,  369367448)  and  since
       2.5.71  also  LINUX_REBOOT_MAGIC2C  (that  is,
       537993216) are permitted as values for magic2.
       (The hexadecimal values of these constants are
       meaningful.)
 * 最后一句也提到了十六进制
 * LINUX_REBOOT_MAGIC1 即 “feeldead”, 而LINUX_REBOOT_MAGIC2的含义,根据
 * https://stackoverflow.com/questions/4808748/magic-numbers-of-the-linux-reboot-system-call
 * 得知是Linus Torvalds和家人的生日.
 * 
 * 尝试后发现没有办法以man中的int  reboot(int  magic,  int  magic2, int cmd,
       void *arg);进行系统调用,只能用glibc的wrapper:
 * int reboot(int cmd);
 * cmd为RB_AUTOBOOT时指定操作为重启
 * 
 * 下面的代码由man的提示得出
 */

#include <sys/reboot.h>
#include <unistd.h>

int main() {
    reboot(RB_AUTOBOOT);

    return 0;
}