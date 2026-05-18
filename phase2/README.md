# Phase 2 — Kernel Threads and Shell Utilities

**Architecture:** xv6 on RISC-V (64-bit)
**Course:** CMPS 240, AUB, Fall 2025
**Build:** `make qemu` (requires `riscv64-unknown-elf-gcc` or `riscv64-linux-gnu-gcc` and `qemu-system-riscv64`)

This phase adds kernel-thread support, a small user-space thread library,
and three shell utilities (`tree`, `diff`, command history).

## What's new

### `clone()` system call — `kernel/proc.c`, `kernel/sysproc.c`

A new system call analogous to `fork()`, except the child shares the
parent's address space (page table, heap, file table). This is the kernel
primitive needed for real threads: same memory, separate execution.

The signature:

```c
int clone(void (*fn)(void *), void *arg, void *stack);
```

The kernel allocates a new `proc` struct, copies the parent's page table
*by reference* rather than by copy, sets up the trap frame so the child
returns into `fn(arg)` on the user-supplied stack, and marks it `RUNNABLE`.

### User-space thread library — `user/uthread.h`, `user/uthread.c`

Wraps `clone()` into a `pthreads`-style API:

```c
int  thread_create(void (*fn)(void *), void *arg);
void thread_join(int pid);
```

`thread_create` allocates a 4KB stack from the user heap, calls `clone()`,
and returns the new thread's PID. `thread_join` waits for a specific PID
to terminate using `wait()` in a loop.

### Ticket lock — `user/uthread.h`, `user/uthread.c`

A FIFO-fair spinlock built on GCC atomic primitives, not a plain
test-and-set:

```c
typedef struct { uint ticket; uint turn; } ticketlock;

void lock_init(ticketlock *lk);
void lock_acquire(ticketlock *lk);
void lock_release(ticketlock *lk);
```

`lock_acquire` does `__sync_fetch_and_add(&lk->ticket, 1)` to grab a
ticket, then spins until `lk->turn` matches. `lock_release` does
`__sync_fetch_and_add(&lk->turn, 1)` to hand off to the next waiter.

The fairness property matters: under contention, threads are served in
the order they arrived, so no starvation. A naïve test-and-set lock
gives no such guarantee.

### Shell utilities

- **`user/tree.c`** — recursively prints a directory tree with indent
  per depth level.
- **`user/diff.c`** — line-by-line file comparison; reports the first
  divergent line and remaining mismatches.
- **Command history in `user/sh.c`** — up/down arrow keys recall
  previous commands from a 20-entry circular buffer. Implemented in
  `add_history()` and the `hist_idx` navigation in the shell's main
  read loop.

## Tests

Three test programs live in `user/`:

- **`clonetest.c`** — spawns multiple threads and verifies they observe
  the same global variables (proves shared address space).
- **`locktest.c`** — spawns N threads that each increment a shared
  counter M times; without the lock, the result is wrong due to races;
  with the lock, the final count is exactly N×M.
- **`difftest.c`** — exercises `diff` against pairs of files with
  known divergences.

Run them from the xv6 shell after `make qemu`:

```
$ clonetest
$ locktest
$ difftest
```

## Code layout

```
phase2/
└── xv6-riscv/
    ├── kernel/
    │   ├── proc.c          ← clone() implementation
    │   ├── sysproc.c       ← sys_clone wrapper
    │   ├── syscall.c       ← syscall table entry
    │   └── syscall.h       ← SYS_clone number
    ├── user/
    │   ├── uthread.h       ← thread + lock API
    │   ├── uthread.c       ← thread + ticket lock implementation
    │   ├── tree.c          ← tree command
    │   ├── diff.c          ← diff command
    │   ├── sh.c            ← shell + command history
    │   ├── clonetest.c     ← thread tests
    │   ├── locktest.c      ← lock contention tests
    │   └── difftest.c      ← diff tests
    └── Makefile            ← updated to compile new programs
```

## Notes on the architecture choice

This phase uses xv6 on **RISC-V** rather than the original x86 build
(which Phase 3 returns to). RISC-V xv6 is MIT's actively-maintained
fork; the kernel API surface is similar but the trap and context-switch
paths look quite different. Implementing `clone()` here meant working
inside the RISC-V `trapframe` and `swtch` paths, not the x86 ones.
