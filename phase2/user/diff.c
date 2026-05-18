#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXLINES 512
#define MAXLEN   256

char lines1[MAXLINES][MAXLEN];
char lines2[MAXLINES][MAXLEN];

// Read all lines from fd into buffer, return count
int readlines(int fd, char buf[][MAXLEN], int max) {
    int n = 0, i = 0;
    char c;
    while (n < max) {
        i = 0;
        while (read(fd, &c, 1) == 1) {
            if (c == '\n' || i >= MAXLEN - 1) {
                buf[n][i] = '\0';
                n++;
                break;
            }
            buf[n][i++] = c;
        }
        if (i == 0) break; // EOF
    }
    return n;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: diff file1 file2\n");
        exit(1);
    }

    int fd1 = open(argv[1], 0);
    int fd2 = open(argv[2], 0);

    if (fd1 < 0) { fprintf(2, "diff: cannot open %s\n", argv[1]); exit(1); }
    if (fd2 < 0) { fprintf(2, "diff: cannot open %s\n", argv[2]); exit(1); }

    int n1 = readlines(fd1, lines1, MAXLINES);
    int n2 = readlines(fd2, lines2, MAXLINES);
    close(fd1);
    close(fd2);

    int max = n1 > n2 ? n1 : n2;
    int diffs = 0;

    for (int i = 0; i < max; i++) {
        char *l1 = (i < n1) ? lines1[i] : "(none)";
        char *l2 = (i < n2) ? lines2[i] : "(none)";
        if (strcmp(l1, l2) != 0) {
            printf("Line %d:\n", i + 1);
            printf("< %s\n", l1);
            printf("> %s\n", l2);
            diffs++;
        }
    }

    if (diffs == 0) printf("Files are identical.\n");
    exit(0);
}