/* 使用线程特有数据重新实现线程安全版的函数 dirname()和 basename()（18.14 节）。 */

/* 
manual page的第三个section是系统调用,可以用man 3 dirname/basename查看这两个调用.
一些有用的信息:

Both  dirname()  and basename() may modify the
contents of path, so it may  be  desirable  to
pass  a  copy  when calling one of these func‐
tions.

These functions may return pointers to  stati‐
cally  allocated memory which may be overwrit‐
ten by subsequent calls.  Alternatively,  they
may  return a pointer to some part of path, so
that the string referred to by path should not
be  modified  or  freed  until the pointer re‐
turned by the function is no longer required.

在这里可以简单地解读为这两个调用和书上举的strerror()的例子一样,只需要把他们返回的
指针指向的字符串拷贝到线程特有数据存起来就行了.
因为懒得重新写一个dirname,就直接调用原本的函数了.

下面两种输出结果分别是注释和不注释掉my_dirname()的第一行的结果:

[VM-4-12-ubuntu:31]$ ./31.2
Main thread has called my_dirname()
Other thread about to call my_dirname()
Other thread: str (0x55c0a6274020) = hoho/ok
Main thread: str (0x55c0a6274010) = haha/good
[VM-4-12-ubuntu:31]$ ./31.2
Main thread has called my_dirname()
Other thread about to call my_dirname()
Other thread: str (0x7fe374000b60) = hoho/ok
Main thread: str (0x5575a1c562a0) = haha/good

因为dirname本身其实并不总是把结果存放在同一个缓冲区,所以两次都是正确的输出.
但是看两次输出的地址可以看出区别,原本的dirname()可能是返回了p1和p2,my_dirname()
则是返回了线程特有数据的指针.
 */

#include <pthread.h>
#include "tlpi_hdr.h"
#include <libgen.h>

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t xname_key;
#define MAX_NAME_LEN 256

char p1[] = "haha/good/bad";
char p2[] = "hoho/ok/bad";

static void destructor(void *buf) {
    free(buf);
}

static void create_key() {
    int s = pthread_key_create(&xname_key, destructor);
    if (s != 0) errExitEN(s, "pthread_key_create");
}

char *my_dirname(char *path) {
    // return dirname(path);
    int s = pthread_once(&once, create_key);
    if (s != 0) errExitEN(s, "pthread_once");

    char *buf = (char*)pthread_getspecific(xname_key);
    if (buf == NULL) {
        buf = (char*)malloc(MAX_NAME_LEN);
        if (buf == NULL) errExit("malloc");
        s = pthread_setspecific(xname_key, buf);
        if (s != 0) errExitEN(s, "pthread_setspecific");
    }

    char *ret = dirname(path);
    strncpy(buf, ret, MAX_NAME_LEN - 1);
    buf[MAX_NAME_LEN - 1] = '\0';

    return buf;
}

// test code

static void *thread_func(void *arg) {
    char *str;
    puts("Other thread about to call my_dirname()");
    str = my_dirname(p2);
    printf("Other thread: str (%p) = %s\n", str, str);
    return NULL;
}

int main() {
    char *str = my_dirname(p1);
    puts("Main thread has called my_dirname()");

    pthread_t t;
    int s = pthread_create(&t, NULL, thread_func, NULL);
    if (s != 0) errExitEN(s, "pthread_create");

    s = pthread_join(t, NULL);
    if (s != 0) errExitEN(s, "pthread_join");
    
    printf("Main thread: str (%p) = %s\n", str, str);

    exit(EXIT_SUCCESS);
}

