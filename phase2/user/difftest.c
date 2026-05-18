#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    int fd;

    // Create file a.txt
    fd = open("a.txt", O_CREATE | O_WRONLY);
    write(fd, "hello\nworld\ntest\n", 16);
    close(fd);

    // Create file b.txt
    fd = open("b.txt", O_CREATE | O_WRONLY);
    write(fd, "hello\ncmps240\ntest\n", 16);
    close(fd);

    char *args[] = {"diff", "a.txt", "b.txt", 0};
    exec("diff", args);
    exit(0);
}
