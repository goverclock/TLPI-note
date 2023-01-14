#include <fcntl.h>
#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <unistd.h>

int main() {
    int fd = open("hole_file", O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd == -1) {
        errExit("open");
    }

    const char *w1 = "hello";
    const char *w2 = "world";
    write(fd, w1, strlen(w1));

    lseek(fd, 100, SEEK_END);
    write(fd, w2, strlen(w2));

    int pos = lseek(fd, 0, SEEK_END);
    printf("%d\n", pos);

    return 0;
}
