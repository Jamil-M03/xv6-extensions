typedef struct {
    volatile int ticket;
    volatile int turn;
} ticketlock;

void lock_init(ticketlock *lk);
void lock_acquire(ticketlock *lk);
void lock_release(ticketlock *lk);
int  thread_create(void(*fn)(void*), void *arg);
void thread_join(int pid);