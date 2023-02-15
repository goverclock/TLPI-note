/* 4.3.2 节曾指出，如果一个文件正处于执行状态，那么要将其打开以执行写操作是
不可能的（open()调用返回−1，且将 errno 置为 ETXTBSY）。然而，在 shell 中执行
如下操作却是可能的：
...(略)
最后一条命令覆盖了现有的同名可执行文件。原因何在？（提示：在每次编译后调
用 ls –li 命令来查看可执行文件的 i-node 编号。） */

/* 
可以看到i-node编号改变了,说明正在执行的文件已经被unlink但还没有删除,并且产生了同名
的新文件,两者并不是同一个文件,所以并没有发生"将其打开以执行写操作".

ubuntu@VM-4-12-ubuntu:~/TLPI-note/18$ ls -li ./longrunner; ./longrunner &
1308701 -rwxrwxr-x 1 ubuntu ubuntu 16744 Feb 13 16:12 ./longrunner
[1] 76871
running.
ubuntu@VM-4-12-ubuntu:~/TLPI-note/18$ vim longrunner.c ; cc -o longrunner longrunner.c ; ls -i longrunner
1308705 longrunner
ubuntu@VM-4-12-ubuntu:~/TLPI-note/18$ done

[1]+  Done                    ./longrunner
ubuntu@VM-4-12-ubuntu:~/TLPI-note/18$
 */

#include <unistd.h>
#include <stdio.h>

int main() {
    puts("runnisssssng.");
    sleep(10);
    puts("done");

    return 0;
}
