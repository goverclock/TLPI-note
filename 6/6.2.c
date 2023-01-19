/* 编写一个程序，观察当使用 longjmp()函数跳转到一个已经返回的函数时会发生
什么？ */

/* 
$ ./6.2 
Segmentation fault (core dumped)
 */
#include "tlpi_hdr.h"
#include <setjmp.h>

static jmp_buf env;

void functojmp();

int main() {
    functojmp();
    longjmp(env, 1);

    return 0;
}

void functojmp() {
    setjmp(env);
}
