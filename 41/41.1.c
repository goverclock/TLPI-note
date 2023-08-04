/* 在使用和不使用–static选项的情况下编译一个程序来看看动态地与C库进行链接的
程序与静态地与 C 库进行链接的程序在大小方面的差别。 */

/* 
直接采用39.1.c.

$ gcc ./41.1.c -ltlpi -lcap
$ ls -lh a.out 
-rwxr-xr-x 1 orangepi orangepi 30K Aug  5 07:51 a.out
$ gcc ./41.1.c -ltlpi -lcap -static
$ ls -lh a.out 
-rwxr-xr-x 1 orangepi orangepi 617K Aug  5 07:51 a.out
 */

#include <sched.h>
#include <sys/capability.h>
#include "tlpi_hdr.h"

static int modifyCap(int capability, int setting) {
    cap_t caps;
    cap_value_t capList[1];

    /* Retrieve caller's current capabilities */

    caps = cap_get_proc();
    if (caps == NULL) return -1;

    /* Change setting of 'capability' in the effective set of 'caps'. The
       third argument, 1, is the number of items in the array 'capList'. */

    capList[0] = capability;
    if (cap_set_flag(caps, CAP_EFFECTIVE, 1, capList, setting) == -1) {
        cap_free(caps);
        return -1;
    }

    /* Push modified capability sets back to kernel, to change
       caller's capabilities */

    if (cap_set_proc(caps) == -1) {
        cap_free(caps);
        return -1;
    }

    /* Free the structure that was allocated by libcap */

    if (cap_free(caps) == -1) return -1;

    return 0;
}

static int /* Raise capability in caller's effective set */
raiseCap(int capability) {
    return modifyCap(capability, CAP_SET);
}

/* An analogous dropCap() (unneeded in this program), could be
   defined as: modifyCap(capability, CAP_CLEAR); */

static int /* Drop all capabilities from all sets */
dropAllCaps(void) {
    cap_t empty;
    int s;

    empty = cap_init();
    if (empty == NULL) return -1;

    s = cap_set_proc(empty);

    if (cap_free(empty) == -1) return -1;

    return s;
}

int main(int argc, char *argv[]) {
    int j, pol;
    struct sched_param sp;

    if (argc < 3 || strchr("rfo", argv[1][0]) == NULL)
        usageErr(
            "%s policy priority [pid...]\n"
            "    policy is 'r' (RR), 'f' (FIFO), "
#ifdef SCHED_BATCH /* Linux-specific */
            "'b' (BATCH), "
#endif
#ifdef SCHED_IDLE /* Linux-specific */
            "'i' (IDLE), "
#endif
            "or 'o' (OTHER)\n",
            argv[0]);

    pol = (argv[1][0] == 'r')   ? SCHED_RR
          : (argv[1][0] == 'f') ? SCHED_FIFO
          :
#ifdef SCHED_BATCH /* Linux-specific, since kernel 2.6.16 */
          (argv[1][0] == 'b') ? SCHED_BATCH
          :
#endif
#ifdef SCHED_IDLE /* Linux-specific, since kernel 2.6.23 */
          (argv[1][0] == 'i') ? SCHED_IDLE
                              :
#endif
                              SCHED_OTHER;

    sp.sched_priority = getInt(argv[2], 0, "priority");

    if (raiseCap(CAP_SYS_NICE) == -1)
        errExit("raiseCap");

    for (j = 3; j < argc; j++)
        if (sched_setscheduler(getLong(argv[j], 0, "pid"), pol, &sp) == -1)
            errExit("sched_setscheduler");

    if (dropAllCaps() == -1)
        errExit("dropAllCap");

    exit(EXIT_SUCCESS);
}
