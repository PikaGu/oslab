#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path) {
    char* p;
    for (p = path+strlen(path); p >= path && *p != '/'; p--);
    p++;
    return p;
}

void find(char* path, char* name) {
    struct stat st;
    if (stat(path, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        return;
    }

    switch (st.type) {
    case T_DIR:
        int fd;
        if ((fd = open(path, 0)) < 0) {
            fprintf(2, "find: cannot open %s\n", path);
            return;
        }
        
        char buf[512];
        strcpy(buf, path);
        char* p = buf+strlen(buf);
        *p++ = '/';

        struct dirent de;
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) {
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0){
                fprintf(2, "ls: cannot stat %s\n", buf);
                continue;
            }
            if (strcmp(fmtname(p), ".") == 0 || strcmp(fmtname(p), "..") == 0) {
                continue;
            }

            find(buf, name);
        }
        close(fd);
        break;
    
    case T_FILE:
        if (strcmp(fmtname(path), name) == 0) {
            fprintf(1, "%s\n", path);
        }
        break;
    }
}

int
main(int argc, char *argv[])
{
    if(argc < 3){
        fprintf(2, "usage: find path filename\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}
