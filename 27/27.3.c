/* 如果赋予如下脚本可执行权限并以 exec()运行，输出结果如何？
#!/bin/cat -n
Hello world
 */

/* 
cat -n就是把输出的每一行开头带上行号.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/27$ ./27.3
     1  #!/bin/cat -n
     2  Hello worldubuntu@VM-4-12-ubuntu:~/TLPI-note/27$
 */

#include "tlpi_hdr.h"

int main() {
    const char *filename = "./cat.txt";
    execl("./cat.txt", filename, NULL);

    exit(EXIT_SUCCESS);
}
