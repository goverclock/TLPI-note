/* 编写一个使用 syslog(3)的程序（与 logger(1)类似）来将任意的消息写入到系统日志
文件中。程序应该接收包含如记录到日志中的消息的命令行参数，同时应该允许指
定消息的 level。 */

#include <syslog.h>
#include "tlpi_hdr.h"

static void usageError(const char *progName) {
    fprintf(stderr, "Usage: %s [-p] [-e] [-l level] \"message\"\n", progName);
    fprintf(stderr, "    -p   log PID\n");
    fprintf(stderr, "    -e   log to stderr also\n");
    fprintf(stderr, "    -l   level (g=EMERG; a=ALERT; c=CRIT; e=ERR\n");
    fprintf(stderr, "                w=WARNING; n=NOTICE; i=INFO; d=DEBUG)\n");
    exit(EXIT_FAILURE);
}

extern int optind;
extern char *optarg;

int main(int argc, char *argv[]) {
    int options = 0;
    int level = LOG_INFO;

    int opt;
    while ((opt = getopt(argc, argv, "l:pe")) != -1) {
        switch (opt) {
            case 'l':
                switch (optarg[0]) {
                    case 'a':
                        level = LOG_ALERT;
                        break;
                    case 'c':
                        level = LOG_CRIT;
                        break;
                    case 'e':
                        level = LOG_ERR;
                        break;
                    case 'w':
                        level = LOG_WARNING;
                        break;
                    case 'n':
                        level = LOG_NOTICE;
                        break;
                    case 'i':
                        level = LOG_INFO;
                        break;
                    case 'd':
                        level = LOG_DEBUG;
                        break;
                    default:
                        cmdLineErr("Bad facility: %c\n", optarg[0]);
                }
                break;
            case 'p':
                options |= LOG_PID;
                break;
            case 'e':
                options |= LOG_PERROR;
                break;
            default:
                fprintf(stderr, "Bad option\n");
                usageError(argv[0]);
        }
    }

    if (argc != optind + 1) usageError(argv[0]);

    openlog(argv[0], options, LOG_USER);
    syslog(LOG_USER | level, "%s", argv[optind]);
    closelog();

    exit(EXIT_SUCCESS);
}
