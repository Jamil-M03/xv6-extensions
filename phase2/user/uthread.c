// uthread.c
#include "kernel/types.h"
#include "user/user.h"
#include "user/uthread.h"

#define STACK_SIZE 4096

void lock_init(ticketlock *lk) {
    lk->ticket = 0;
    lk->turn   = 0;
}

void lock_acquire(ticketlock *lk) {
    // Fetch-and-increment: get my ticket
    int my_ticket = __sync_fetch_and_add(&lk->ticket, 1);
    // Spin until it's our turn
    while (lk->turn != my_ticket)
        ; // busy-wait
}

void lock_release(ticketlock *lk) {
    __sync_fetch_and_add(&lk->turn, 1);
}

int thread_create(void(*fn)(void*), void *arg) {
    char *stack = malloc(STACK_SIZE);
    if (!stack) return -1;
    return clone(fn, arg, stack);
}

void thread_join(int pid) {
    int wpid;
    while ((wpid = wait(0)) != pid && wpid > 0)
        ;
}