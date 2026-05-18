#include "kernel/types.h"
#include "user/user.h"
#include "user/uthread.h"

ticketlock lk;
int counter = 0;

void increment(void *arg) {
    for (int i = 0; i < 1000; i++) {
        lock_acquire(&lk);
        counter++;
        lock_release(&lk);
    }
    exit(0);
}

int main() {
    lock_init(&lk);

    char *stack1 = malloc(4096);
    char *stack2 = malloc(4096);

    clone(increment, 0, stack1);
    clone(increment, 0, stack2);

    wait(0);
    wait(0);

    printf("counter = %d (expected 2000)\n", counter);
    exit(0);
}
