/* 修改程序清单 7-1 中的程序（free_and_sbrk.c），在每次执行 malloc()后打印出 program
break 的当前值。指定一个较小的内存分配尺寸来运行该程序。这将证明 malloc()不会
在每次被调用时都调用 sbrk()来调整 program break 的位置，而是周期性地分配大块
内存，并从中将小片内存返回给调用者。 */

/* 
$ ./7.1 1000 10240 2 1 1000
nitial program break:          0x55cf50993000
Allocating 1000*10240 bytes
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf50993000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509b6000
sbrk(0) = 0x55cf509d7000
sbrk(0) = 0x55cf509d7000
sbrk(0) = 0x55cf509d7000
...
sbrk(0) = 0x55cf51338000
sbrk(0) = 0x55cf51338000
sbrk(0) = 0x55cf51338000
sbrk(0) = 0x55cf5135b000
Program break is now:           0x55cf5135b000
Freeing blocks from 1 to 1000 in steps of 2
After free(), program break is: 0x55cf5135b000
 */

#define _DEFAULT_SOURCE
#include "tlpi_hdr.h"

#define MAX_ALLOCS 1000000

int
main(int argc, char *argv[])
{
    char *ptr[MAX_ALLOCS];
    int freeStep, freeMin, freeMax, blockSize, numAllocs, j;

    printf("\n");

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s num-allocs block-size [step [min [max]]]\n", argv[0]);

    numAllocs = getInt(argv[1], GN_GT_0, "num-allocs");
    if (numAllocs > MAX_ALLOCS)
        cmdLineErr("num-allocs > %d\n", MAX_ALLOCS);

    blockSize = getInt(argv[2], GN_GT_0 | GN_ANY_BASE, "block-size");

    freeStep = (argc > 3) ? getInt(argv[3], GN_GT_0, "step") : 1;
    freeMin =  (argc > 4) ? getInt(argv[4], GN_GT_0, "min") : 1;
    freeMax =  (argc > 5) ? getInt(argv[5], GN_GT_0, "max") : numAllocs;

    if (freeMax > numAllocs)
        cmdLineErr("free-max > num-allocs\n");

    printf("Initial program break:          %10p\n", sbrk(0));

    printf("Allocating %d*%d bytes\n", numAllocs, blockSize);
    for (j = 0; j < numAllocs; j++) {
        ptr[j] = (char*)malloc(blockSize);
        if (ptr[j] == NULL)
            errExit("malloc");
        void *pos = sbrk(0);
        printf("sbrk(0) = %p\n", pos);
    }

    printf("Program break is now:           %10p\n", sbrk(0));

    printf("Freeing blocks from %d to %d in steps of %d\n",
                freeMin, freeMax, freeStep);
    for (j = freeMin - 1; j < freeMax; j += freeStep)
        free(ptr[j]);

    printf("After free(), program break is: %10p\n", sbrk(0));

    exit(EXIT_SUCCESS);
}
