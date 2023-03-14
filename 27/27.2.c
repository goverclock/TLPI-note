/* 试用execve()实现execlp()。需使用stdarg(3) API来处理execlp()所提供的变长参数列表。
还需要使用 malloc 函数库中函数为参数以及环境向量分配空间。最后，请注意，要检
查特定目录下某个文件是否存在且可以执行，有一种简单方法：尝试执行该文件即可。 */

/* 
太复杂不想写了= =
官方的答案是tlpi-dist/procexec/execlp.c.甚至需要手动实现在PATH变量中搜索的过程.
 */

#include "tlpi_hdr.h"
#include <stdarg.h>
#include <malloc.h>
#include <limits.h>

extern char **environ;

int my_execlp(const char *file, const char *arg, ...) {
    va_list ap;
    va_start(ap, arg);
    char *t;
    puts(arg);
    while (1) {
        arg = va_arg(ap, char*);
        if (arg == NULL) break;
        puts(arg);
    }
    va_end(ap);
    
    // execve(file, arg, environ);
    return 0;
}

int main() {
    my_execlp("a", "qwe", "haha", "heihe", NULL);


    exit(EXIT_SUCCESS);
}
