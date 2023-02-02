// 见13.3~13.4.md

/* 

ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ ./13.4
If I had more time, 
I would have written you a shorter letter.
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ ./13.4 > a
ubuntu@VM-4-12-ubuntu:~/TLPI-note/13$ cat a
I would have written you a shorter letter.
If I had more time,

*/

#include "tlpi_hdr.h"
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("If I had more time, \n");
    write(STDOUT_FILENO, "I would have written you a shorter letter.\n", 43);

    return 0;
}
