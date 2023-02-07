/* 编写 chattr(1)命令的简化版来修改文件的 i 节点标志。参阅 chattr(1)
手册页以掌握 chattr 命令行接口的细节。（无需实现-R、-V、-v 选项。） */

/*
读chattr(1)的时候发现了一个叫project number的东西,书里没有提:
https://superuser.com/questions/1441509/what-are-project-ids-in-linux-as-mentioned-in-the-chattr-manual
project number也属于i节点,如果一个目录设置了project
number,则其下的文件将继承之,借此就可以管理 "project quota",而这是一种管理"disk
quota"的方式. 书上没有相关内容,以后再了解.

只实现到了这种程度:
Usage: chattr [-+=aAcCdDeijPsStTuF] files...
这些选项中的一部分在书出版的时候还是没有的.
可以在ioctl_iflags(2)查看选项对应的标志.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ lsattr ./file 
--------------e----- ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ ./15.7 +aAc file ERROR [EPERM Operation not permitted] ioctl2
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ sudo ./15.7 +aAc file 
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ lsattr ./file 
-----a-Ac-----e----- ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ sudo ./15.7 -ac file 
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ lsattr ./file 
-------A------e----- ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ sudo ./15.7 =T file 
ERROR [EOPNOTSUPP/ENOTSUP Operation not supported] ioctl2
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ sudo ./15.7 =a file 
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ lsattr ./file 
-----a-------------- ./file

注意,有些标志位需要特权才能设置,并不是说进程的有效用户ID为文件的属主就可以随意地设置标志.

我额外做了些测试:
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ lsattr ./file
-----a--------e----- ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ ll ./file
-rw-r--r-- 1 ubuntu ubuntu 0 Feb  7 10:06 ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ chmod a-r ./file
chmod: changing permissions of './file': Operation not permitted
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ sudo chmod a-r ./file
chmod: changing permissions of './file': Operation not permitted
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ chattr -a ./file
chattr: Operation not permitted while setting flags on ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ sudo chattr -a ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ lsattr ./file
--------------e----- ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ chmod a-r ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ ll ./file
--w------- 1 ubuntu ubuntu 0 Feb  7 10:06 ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ lsattr ./file
lsattr: Permission denied While reading flags on ./file
ubuntu@VM-4-12-ubuntu:~/TLPI-note/15$ sudo lsattr ./file
--------------e----- ./file

有两个结论:
1. inode中的a标志位表示仅能在尾部追加,使得所有对文件的更新都只能为追加到尾部.这对元数据也适用,后果
    就是一般进程无法修改文件权限(chmod失败).
2. 文件权限(rwx)也对inode中的元数据适用,如果对文件没有r权限,则不能读取元数据如inode标志(lsattr).

只是想确认一下元数据本身是否受到这些限制.

 */

#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    const char *usage = "%s [-+=aAcCdDeijPsStTuF] files...\n";
    if (argc < 3) usageErr(usage, argv[0]);
    if (strchr("-+=", argv[1][0]) == NULL) usageErr(usage, argv[0]);

    int attr = 0;

    for (char *p = &argv[1][1]; *p != '\0'; p++) {
        switch (*p) {
            case 'a':
                attr |= FS_APPEND_FL;
                break;
            case 'A':
                attr |= FS_NOATIME_FL;
                break;
            case 'c':
                attr |= FS_COMPR_FL;
                break;
            case 'C':
                attr |= FS_NOCOW_FL;
                break;  // new!!
            case 'd':
                attr |= FS_NODUMP_FL;
                break;
            case 'D':
                attr |= FS_DIRSYNC_FL;
                break;
            // case 'e': attr |= FS_xxxxxxx;        break;      // didn't find
            case 'i':
                attr |= FS_IMMUTABLE_FL;
                break;
            case 'j':
                attr |= FS_JOURNAL_DATA_FL;
                break;
            case 'P':
                attr |= FS_PROJINHERIT_FL;
                break;  // new!!
            case 's':
                attr |= FS_SECRM_FL;
                break;
            case 'S':
                attr |= FS_SYNC_FL;
                break;
            case 't':
                attr |= FS_NOTAIL_FL;
                break;
            case 'T':
                attr |= FS_TOPDIR_FL;
                break;
            case 'u':
                attr |= FS_UNRM_FL;
                break;
            // case 'F': attr |= FS_xxxxxxx;           break;   // didn't find
            default:
                usageErr(usage, argv[0]);
        }
    }

    for (int i = 2; i < argc; i++) {
        char *path = argv[i];
        int fd = open(path, O_RDONLY);
        if (fd == -1) {
            errMsg("open %s", path);
            continue;
        }

        if (argv[1][0] != '=') {
            int old_attr;
            if (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) == -1) errExit("ioctl1");
            if (argv[1][0] == '-')
                attr = old_attr & ~attr;
            else // == '+'
                attr |= old_attr;
        }
        if (ioctl(fd, FS_IOC_SETFLAGS, &attr) == -1)
            errExit("ioctl2");
        if (close(fd) == -1)
            errExit("close");
    }

    return 0;
}
