/* 
这个程序用来测试在open和read调用之间,目标文件消失的情况.
结果发现read可以正常地读取已被手动删除的文件中的内容.

"If the file is deleted, the file handle remains open and can still be used 
(This is not what some people expect). The file will not really be deleted 
until the last handle is closed."
参考:https://stackoverflow.com/questions/2028874/what-happens-to-an-open-file-handle-on-linux-if-the-pointed-file-gets-moved-or-d

即,在open调用成功后不需要考虑文件消失的情况,因为read调用不会受此影响.
 */

#include <sys/fcntl.h>
#include "tlpi_hdr.h"

int main() {
    int fd = open("testfile", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        errExit("open");
    }
    // write something to it
    const char *txt = "hello";
    if (write(fd, txt, strlen(txt)) == -1) {
        errExit("write");
    }

    puts("now delete 'testfile' manually and press enter");
    getchar();
    puts("trying to read the deleted file");
    
    char buf[256];
    int num_read = 0;
    if(lseek(fd, SEEK_SET, 0) == -1) {
        errExit("lseek");
    }
    if((num_read = read(fd, buf, sizeof(buf))) == -1) {
        errExit("read");
    }
    printf("%d\n", num_read);
    buf[num_read] = '\0';
    puts("read successfully");
    puts(buf);

    return 0;
}
