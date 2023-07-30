/* 实现 nice(1)命令。 */

/* 
$ nice --help
Usage: nice [OPTION] [COMMAND [ARG]...]
Run COMMAND with an adjusted niceness, which affects process scheduling.
With no COMMAND, print the current niceness.  Niceness values range from
-20 (most favorable to the process) to 19 (least favorable to the process).

Mandatory arguments to long options are mandatory for short options too.
  -n, --adjustment=N   add integer N to the niceness (default 10)
      --help     display this help and exit
      --version  output version information and exit

getopt()用起来实在是麻烦,所以实现了简化版,直接把argv[1]当成-n参数了.

使用fork()创建子进程时会继承nice值,且在exec()调用中保持.所以nice程序只需要设置好nice值,
然后直接用execv()就可以了,没有必要fork().

$ ./a.out 11 ./a.out -2 ./a.out 
Permission denied
11

 */

#include "tlpi_hdr.h"
#include <sys/resource.h>
#include <string.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    int cur_nice = getpriority(PRIO_PROCESS, 0);
    
    // simply print the nice value
    if (argc == 1) {
        printf("%d\n", cur_nice);
        exit(EXIT_SUCCESS);
    }
    if (!strcmp(argv[1], "--help")) {
        usageErr("lol just read the manual.\n");
    }
    if (argc < 3) {
        fprintf(stderr, "a command must be given with an adjustment.\n");
        exit(EXIT_FAILURE);
    }
    
    errno = 0;
    int inc = strtol(argv[1], NULL, 10);
    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    setpriority(PRIO_PROCESS, 0, inc + cur_nice);
    if (errno != 0) {
        puts(strerror(errno));
    }

    execvp(argv[2], argv + 2);
    puts(strerror(errno));

    exit(EXIT_SUCCESS);
}
