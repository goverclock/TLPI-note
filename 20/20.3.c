/* 编写一程序，以 sigaction()来建立信号处理器函数，请验证 SA_RESETHAND 和
SA_NODEFER 标志的效果。 */

/*
flags为0-默认行为

ubuntu@VM-4-12-ubuntu:~/TLPI-note/20$ ./20.3 0
^Center- 0
^C^C^C^C^C^C^C^C^C^C^C^C^Creturn-0
  enter- 1
  return-1

SA_RESETHAND-当捕获该信号时，会在调用处理器函数之前将信号处置重置为默认值（即
SIG_DFL）

这句话很容易让人误会,应该注释一句:这次捕获信号不会产生默认值行为,仍然会调用处理器函数.

ubuntu@VM-4-12-ubuntu:~/TLPI-note/20$ ./20.3 1
^Center- 0
^C^C^C^C^C^C^C^C^C^C^C^Creturn-0

SA_NODEFER-捕获该信号时，不会在执行处理器程序时将该信号自动添加到进程掩码中。

ubuntu@VM-4-12-ubuntu:~/TLPI-note/20$ ./20.3 2
^Center- 0
^C  enter- 1
^C    enter- 2
^C      enter- 3
^C        enter- 4
^C          enter- 5
^C            enter- 6
^C              enter- 7
^C                enter- 8
^C                  enter- 9
^C                    enter- 10
^C                      enter- 11
^C                        enter- 12
^C                          enter- 13
^C                            enter- 14
^C                              enter- 15
^C                                enter- 16
^C                                  enter- 17
                                  return-17
                                return-16
                              return-15
                            return-14
                          return-13
                        return-12
                      return-11
                    return-10
                  return-9
                return-8
              return-7
            return-6
          return-5
        return-4
      return-3
    return-2
  return-1
return-0
 */

#include "tlpi_hdr.h"
#include <signal.h>

static void handler(int sig) {
    static int cnt = 0;

    int n = cnt++;
    printf("%*senter- %d\n", n * 2, "", n);
    sleep(10);  // another same signal might be captured here
    printf("%*sreturn-%d\n", n * 2, "", n);
}

int main(int argc, char *argv[]) {
    if (argc != 2) usageErr("%s 0/1/2\n", argv[0]);
    int mode = atoi(argv[1]);

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_handler = handler;
    if (mode == 0)
        act.sa_flags = 0;
    else if (mode == 1)
        act.sa_flags = SA_RESETHAND;
    else if (mode == 2)
        act.sa_flags = SA_NODEFER;
    else
        usageErr("%s 0/1/2\n", argv[0]);


    if (sigaction(SIGINT, &act, NULL) == -1) errExit("sigaction SIGINT");

    pause();

    return 0;
}
