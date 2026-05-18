#include "kernel/types.h"
#include "user/user.h"

int shared = 0; // shared variable between parent and thread

void thread_fn(void *arg) {
    shared = 42;
    printf("thread: set shared to %d\n", shared);
    exit(0);
}

int main() {
    char *stack = malloc(4096);
    if (stack == 0) {
        printf("malloc failed\n");
        exit(1);
    }

    printf("parent: shared = %d before clone\n", shared);

    int pid = clone(thread_fn, 0, stack);
    if (pid < 0) {
        printf("clone failed\n");
        exit(1);
    }

    wait(0); // wait for thread to finish

    printf("parent: shared = %d after thread ran\n", shared);
    // If shared == 42, the thread successfully wrote to shared memory
    exit(0);
}
