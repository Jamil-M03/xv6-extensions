#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void tree(char *path, int depth) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "tree: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "tree: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
    case T_FILE:
        // Print indentation
        for (int i = 0; i < depth; i++) printf("|   ");
        printf("|-- %s\n", path + (depth > 0 ? strlen(path) - strlen(path) : 0));
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            fprintf(2, "tree: path too long\n");
            break;
        }
        // Read directory entries
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) continue;
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;

            // Print indentation + entry name
            for (int i = 0; i < depth; i++) printf("|   ");
            printf("|-- %s\n", de.name);

            // Build full path and recurse if directory
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            // Stat to check if it's a subdirectory
            int subfd = open(buf, 0);
            if (subfd >= 0) {
                struct stat subst;
                if (fstat(subfd, &subst) == 0 && subst.type == T_DIR) {
                    close(subfd);
                    tree(buf, depth + 1);
                    continue;
                }
                close(subfd);
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(".\n");
        tree(".", 0);
    } else {
        printf("%s\n", argv[1]);
        tree(argv[1], 0);
    }
    exit(0);
}