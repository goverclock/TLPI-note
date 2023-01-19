/* 使用 getenv()函数、putenv()函数，必要时可直接修改 environ，来实现 setenv()函数
和 unsetenv()函数。此处的 unsetenv()函数应检查是否对环境变量进行了多次定义，
如果是多次定义则将移除对该变量的所有定义（glibc 版本的 unsetenv()函数实现了这
一功能）。 */

/* 
getenv(name) - 根据名称获取环境变量value
putenv(str) - 使字符串str="name=value"成为环境变量,(glib)如果为"name="则可移除环境变量
                不会为str分配内存

int setenv(const char *name, const char *value, int overwrite) - 会分配内存
int unsetenv(const char *name) - 移除环境变量
 
注意: 如果setenv和putenv发现environ为NULL,则会创建一个新的环境列表,使environ指向此列表
 */

#include "tlpi_hdr.h"
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>

extern char **environ;

int my_setenv(const char*, const char *, int);
int my_unsetenv(const char *);
void printenv(const char*);

int main() {
    // clear environment
    clearenv();
    printenv("init");
    
    my_setenv("hello", "world", 0);
    my_setenv("goodbye", "world", 0);
    my_setenv("goodbye", "heaven", 1);
    my_setenv("goodbye", "hell", 0);
    printenv("set hello and goodbye");

    my_unsetenv("hello");
    printenv("unset hello");

    return 0;
}

int my_setenv(const char* name, const char *value, int overwrite) {
    if (environ == NULL) {
        environ = (char**)malloc(1);
        if (environ == NULL) {
            return -1;
        }
        *environ = NULL;
    }    

    // 1. if name already exist and not overwrite, do nothing
    if (overwrite == 0 && getenv(name) != NULL) {
        return -1;
    } 

    // 2. else add "name=value" to environment
    size_t ln = strlen(name);
    size_t lv = strlen(value);
    char *new_env = (char*)malloc(ln + lv + 1);
    if (new_env == NULL) {
        return -1;
    }
    memcpy(new_env, name, ln);
    new_env[ln] = '=';
    memcpy(new_env + ln + 1, value, lv);
    return putenv(new_env);
}

int my_unsetenv(const char *name) {
    if (environ == NULL) {
        return 0;
    }
    
    int ret = -1;
    size_t len = strlen(name);
    for (char **ep = environ; *ep != NULL; ep++) {
        if (memcmp(name, *ep, len) == 0) {
            *ep[0] = '\0';
            ret = 0;
        }
    }
    
    return ret;
}

void printenv(const char *msg) {
    puts(msg);
    for (char **ep = environ; ep != NULL && *ep != NULL; ep++) {
        puts(*ep);
    }
}
