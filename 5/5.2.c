/* 编写一个程序，使用 O_APPEND 标志并以写方式打开一个已存在的文件，且将
文件偏移量置于文件起始处，再写入数据。数据会显示在文件中的哪个位置？为
什么？ */

/* 

这段程序进行下面的操作:
1. 创建一个文件,写入内容"hello!"
2. 设置描述符的偏移量为0,即文件起始处
3. write调用写"world"
最后得到的文件内容为"hello!world"
即使设置了偏移量到文件起始,新写入的数据仍然出现在文件的末尾,因为文件描述符具有O_APPEND标志位,这保证了设置偏移量和写操作为原子操作.

If the O_APPEND flag of the file status flags is set, the file offset shall be set to the end of the file prior to each write and no intervening file modification operation shall occur between changing the file offset and the write operation.

 */
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tlpi_hdr.h"

int main() {
    // create the file and write something to it
    int fd = open("f", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        errExit("open");
    }
    const char *w1 = "hello!";
    int num_write = write(fd, w1, strlen(w1));
    if (num_write == -1) {
        errExit("write");
    }
    close(fd);

    // open the existing file
    fd = open("f", O_WRONLY | O_APPEND);
    if (fd == -1) {
        errExit("open");
    }

    int pos = lseek(fd, 0, SEEK_SET);   // offset set to begining of the file
    if (pos == -1) {
        errExit("lseek");
    }

    const char *w2 = "world";
    num_write = write(fd, w2, strlen(w2));
    if (num_write == -1) {
        errExit("write");
    }
    return 0;
}
