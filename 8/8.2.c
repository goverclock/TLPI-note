/* 使用 setpwent()、getpwent()和 endpwent()来实现 getpwnam()。 */

/* 
getpwnam - 通过名称返回一个静态passwd结构的指针,不可重入
            使用errno判断返回结果NULL表示出错还是未发现匹配记录
 */

#include "tlpi_hdr.h"
#include <pwd.h>

struct passwd *my_getpwnam(const char *);

int main() {
    struct passwd *pwd = my_getpwnam("root");
    printf("user %s has uid %d\n", pwd->pw_name, pwd->pw_uid);

    return 0;
}

struct passwd *my_getpwnam(const char *name) {
    struct passwd *pwd = NULL;
    while ((pwd = getpwent()) != NULL) {
        if (strcmp(pwd->pw_name, name) == 0 ) {
            break;
        }
    }
    endpwent();
    return pwd;
}
