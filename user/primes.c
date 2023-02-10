#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
next_pipe(int fd) {
    int n;
    read(fd, &n, sizeof(int));
    if (n == 0) {
        return;
    }
    printf("prime %d\n", n);

    int p[2];
    pipe(p);
    if (fork() == 0) {
        close(fd);
        close(p[1]);
        next_pipe(p[0]);
    } else {
        close(p[0]);
        int m;
        while (read(fd, &m, sizeof(int))) {
            if (m % n == 0) {
                continue;
            }
            write(p[1], &m, sizeof(int));
        }
        close(p[1]);
        close(fd);
        wait(0);
    }
}

int
main(int argc, char *argv[])
{
    int p[2];
    pipe(p);

    if (fork() == 0) {
        close(p[1]);
        next_pipe(p[0]);
    } else {
        close(p[0]);
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        wait(0);
    }

    exit(0);
}
