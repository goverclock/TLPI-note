/* 执行下列代码时，将会发现，尽管这两个用户在密码文件中对应不同的 ID，但该
程序的输出还是会将同一个数字显示两次。请问为什么？ */

/* 
因为这个函数是不可重入的.
原题有误,这里是勘误表中给出的正确代码示例.

$ ./8.1 1000 4
ubuntu ubuntu
$ ./8.1 4 1
sync sync
$ ./8.1 5 6
games games
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>

int
main(int argc, char *argv[])
{
    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage: %s uid1 uid2\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%s %s\n", getpwuid(atoi(argv[1]))->pw_name,
                      getpwuid(atoi(argv[2]))->pw_name);

    exit(EXIT_SUCCESS);
}