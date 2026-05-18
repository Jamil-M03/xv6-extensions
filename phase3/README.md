# Phase 3 — MLFQ Scheduler and File-Search Utilities

**Architecture:** xv6 on x86 (32-bit)
**Course:** CMPS 240, AUB, Fall 2025
**Build:** `make qemu-nox` (requires `gcc-i686-linux-gnu` and `qemu-system-x86`)

This phase replaces xv6's default round-robin scheduler with a
Multi-Level Feedback Queue, and adds three file-search utilities.

## Multi-Level Feedback Queue scheduler

### The idea

The default xv6 scheduler is plain round-robin: every `RUNNABLE` process
gets the same time slice, in the order they appear in the process table.
Fair on average, but bad for two real-world cases — interactive jobs
(latency-sensitive, short bursts) and long batch jobs (CPU-bound,
benefits from large quanta) end up treated identically.

MLFQ solves this by maintaining several priority queues with different
time quanta, automatically demoting CPU hogs to lower priorities and
periodically boosting everyone back up to prevent starvation.

### Implementation summary

Three priority queues (`NQUEUE = 3` in `proc.h`), highest priority is
queue 0:

- **Queue 0** — 1-tick quantum, scheduled first
- **Queue 1** — 2-tick quantum
- **Queue 2** — 4-tick quantum, scheduled last

The quantum is `1 << priority`, so each level doubles the previous one.
On every timer tick, the running process's `ticks_used` increments; when
it reaches its queue's quantum, the process is demoted one level (unless
already at queue 2).

To prevent CPU-bound jobs from starving forever at queue 2, a periodic
**priority boost** runs every `BOOST_INTERVAL = 100` ticks: every
non-`UNUSED` process is moved back to queue 0 with its counters reset.
This bounds the worst-case wait for any process.

### Where to look

| File | What changed |
|---|---|
| `proc.h` | Added `NQUEUE` / `BOOST_INTERVAL` constants and four MLFQ fields on `struct proc`: `priority`, `ticks_used`, `total_ticks`, `wait_ticks` |
| `proc.c` | Rewrote `scheduler()` to scan queues high→low, added `mlfq_tick()` called from the timer trap to charge ticks and trigger demotion / boost |
| `trap.c` | Hooked `mlfq_tick()` into the timer interrupt path |
| `allocproc` | Initialises new processes at priority 0 (highest) |

### Key snippet

```c
// proc.c — scheduler() inner loop
for (q = 0; q < NQUEUE && chosen == 0; q++) {
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state != RUNNABLE) continue;
        if (p->priority != q) continue;
        chosen = p;
        break;
    }
}
```

```c
// proc.c — mlfq_tick(), charges the running proc and handles demotion
int quantum = 1 << p->priority;
if (p->ticks_used >= quantum) {
    if (p->priority < NQUEUE - 1)
        p->priority++;
    p->ticks_used = 0;
}
```

## File-search utilities

### `find` — recursive pattern search

```
$ find <path> <pattern>
```

Walks the directory tree rooted at `<path>` and prints every file whose
basename matches `<pattern>`. The pattern supports `*` as a wildcard via
a small recursive matcher (`wmatch` in `find.c`).

### `split` — chunked file copy

```
$ split <file> <chunk_size_bytes>
```

Splits a large file into `<file>.part0`, `<file>.part1`, …, each at most
`<chunk_size_bytes>` bytes. Useful for moving files across filesystems
with size limits or for parallel processing.

### `search` — in-file keyword grep

```
$ search <keyword> <file>
```

Prints every line of `<file>` containing `<keyword>` as a substring.
Simpler than `grep` (no regex), but enough for the common case.

## Tests

After `make qemu-nox`, from the xv6 shell:

```
$ find / README          # find every file named "README"
$ split README 100       # split README into 100-byte chunks
$ search xv6 README      # grep-style search
```

The MLFQ scheduler is exercised by running CPU-bound and short
interactive workloads in parallel and observing that interactive ones
maintain low latency (priority stays at 0) while CPU-bound ones get
demoted but recover at the next boost.

## Code layout

```
phase3/
└── xv6-phase3/
    ├── proc.h          ← MLFQ constants and per-proc fields
    ├── proc.c          ← scheduler() and mlfq_tick()
    ├── trap.c          ← timer trap hooks mlfq_tick()
    ├── find.c          ← find command (recursive wildcard)
    ├── split.c         ← split command (chunked copy)
    ├── search.c        ← search command (substring grep)
    ├── Makefile        ← updated to build new user programs
    └── ... (rest of upstream xv6)
```
