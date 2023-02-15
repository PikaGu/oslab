#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int readline(int fd, char* buf) {
    int n = 0;
    while (read(fd, buf+n, 1)) {
        if (*(buf+n) == '\n' || *(buf+n) == '\0') {
            break;
        }
        n++;
    }
    buf[n] = '\0';
    return n;
}

int
main(int argc, char *argv[])
{
    if(argc < 2){
        fprintf(2, "usage: xargs command [args...]\n");
        exit(1);
    }

    char* args[MAXARG];
    int len = argc - 1;
    memmove(args, argv+1, len * sizeof(char**));

    char* buf = malloc(64);
    int n;
    while ((n = readline(0, buf))) {
        args[len] = buf;
        if (fork() == 0) {
            exec(args[0], args);
            exit(0);
        } else {
            wait(0);
        }
    }
    free(buf);

    exit(0);
}
