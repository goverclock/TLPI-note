/* 编写一程序，试对在单目录下创建和删除大量 1 字节文件所需的时间进行度量。该
程序应以 xNNNNNN 命名格式来创建文件，其中 NNNNNN 为随机的 6 位数字。文
件的创建顺序与生成文件名相同，为随机方式，删除文件则按数字升序操作（删除
与创建的顺序不同）。文件的数量（FN）和文件所在目录应由命令行指定。针对不
同的 NF 值（比如，在 1000 和 20000 之间取值）和不同的文件系统（比如 ext2、
ext3 和 XFS）来测量时间。随着 NF 的递增，每个文件系统下耗时的变化模式如何？
不同文件系统之间，情况又是如何呢？如果按数字升序来创建文件（x000000、
x000001、x0000002 等），然后以相同顺序加以删除，结果会改变吗？如果会，原
因何在？此外，上述结果会随文件系统类型的不同而改变吗？ */

/*
Usage: ./14.1 path-to-dir filenumber - random
       ./14.1 path-to-dir filenumber x - sequential
这个程序只负责在指定目录下创建指定数量的文件并删除.我本来想用系统调用检查一下目标目录的文件系统类型的,
但是发现这样比较麻烦就没做= =
判断一个目录所处的文件系统是通过mount和df两个命令完成的.
随机生成文件时,采用了穷举法删除文件.
参见:https://stackoverflow.com/questions/48319246/how-can-i-determine-filesystem-type-name-with-linux-api-for-c

使用shell命令time计时.

a) 在ext4下:

ubuntu@VM-4-12-ubuntu:~/TLPI-note/14$ time ./14.1 . 20000 x
cwd: /home/ubuntu/TLPI-note/14
creating file...
removing file...

real    0m1.429s
user    0m0.035s
sys     0m0.580s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/14$ time ./14.1 . 20000
cwd: /home/ubuntu/TLPI-note/14
creating file...
removing file...

real    0m1.604s
user    0m0.017s
sys     0m0.663s

顺序的稍快一些.

b) 当我想利用/run测试tmpfs时:
sudo time ./14.1 /run 20000
cwd: /run
creating file...
removing file...
0.04user 0.72system 0:00.77elapsed 98%CPU (0avgtext+0avgdata 1580maxresident)k
0inputs+0outputs (0major+83minor)pagefaults 0swaps

结果发现其实这样就可以了,我对shell的了解太少了:
ubuntu@VM-4-12-ubuntu:~/TLPI-note/14$ time sudo ./14.1 /run 20000
cwd: /run
creating file...
removing file...

real    0m0.752s
user    0m0.034s
sys     0m0.710s
感觉20000这个数值不够大,我就当作顺序的要快一些了.
ubuntu@VM-4-12-ubuntu:~/TLPI-note/14$ time sudo ./14.1 /run 20000 x
cwd: /run
creating file...
removing file...

real    0m0.891s
user    0m0.037s
sys     0m0.835s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/14$ time sudo ./14.1 /run 20000 x
cwd: /run
creating file...
removing file...

real    0m0.685s
user    0m0.026s
sys     0m0.640s
ubuntu@VM-4-12-ubuntu:~/TLPI-note/14$ time sudo ./14.1 /run 20000 x
cwd: /run
creating file...
removing file...

real    0m0.742s
user    0m0.032s
sys     0m0.698s


c) 在/proc目录下出错
time sudo ./14.1 /proc 20000
cwd: /proc
creating file...
ERROR [ENOENT No such file or directory] open

real    0m0.008s
user    0m0.003s
sys     0m0.004s

open似乎没办法在/proc下创建文件,因为它是一个虚拟文件系统,我也不能用mkdir在其下创建一个目录或者用
touch创建一个文件.

 */

#include <limits.h>
#include <math.h>
#include <sys/fcntl.h>

#include "tlpi_hdr.h"

void gene_random_name(char *);
void gene_name(int, char *);

int main(int argc, char *argv[]) {
    const char *usage =
        "./14.1 path-to-dir filenumber - random\n\t./14.1 path-to-dir "
        "filenumber x - sequential\n";
    if (argc != 3 && argc != 4) {
        usageErr(usage);
    }

    // chang working directory
    char *path = argv[1];
    char dir[PATH_MAX];
    if (chdir(path) == -1) errExit("chdir");
    printf("cwd: %s\n", getcwd(dir, PATH_MAX));

    int fn = atoi(argv[2]);
    if (fn > 20000) {
        usageErr(usage);
    }

    char buf[8];
    puts("creating file...");
    for (int i = 0; i < fn; i++) {
        argc == 3 ? gene_random_name(buf) : gene_name(i, buf);
        int fd = open(buf, O_WRONLY | O_CREAT | O_EXCL, 0666);
        if (fd == -1) errExit("open");
        if (write(fd, "1", 1) == -1) errExit("write");
        close(fd);
    }

    // asceding delete
    puts("removing file...");
    int top = argc == 3 ? 20010 : fn;
    for (int i = 0; i < top; i++) {
        gene_name(i, buf);
        unlink(buf);
    }

    return 0;
}

// guarantee no duplicate
void gene_random_name(char *buf) {
    static short used[20010] = {0};
    int n;
    while (1) {
        n = rand() % 20000;
        if (!used[n]) {
            used[n] = 1;
            break;
        }
    }
    gene_name(n, buf);
}

void gene_name(int num, char *buf) { sprintf(buf, "x%06d", num); }
