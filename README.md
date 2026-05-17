# xv6 Extensions

Kernel and user-space extensions to **xv6**, MIT's teaching operating
system, built across three phases for AUB's CMPS 240 (Operating Systems),
Fall 2025. Each phase started from a fresh upstream clone and tackles a
different layer of the OS — system-call surface, kernel threading, and
scheduling.

## Phases at a glance

| Phase | Architecture | Headline work |
|---|---|---|
| **[Phase 1](./phase1/)** | x86 (32-bit) | New system calls, login authentication, shell wildcard expansion |
| **[Phase 2](./phase2/)** | RISC-V (64-bit) | `clone()` syscall + kernel threads, ticket-lock thread library, `tree`/`diff`/history utilities |
| **[Phase 3](./phase3/)** | x86 (32-bit) | Multi-Level Feedback Queue scheduler, `find`/`split`/keyword-search utilities |

Each phase folder is a self-contained xv6 source tree with a phase-specific
README documenting what was added, where in the kernel, and how to build
and test it.

## Course context

CMPS 240 — *Operating Systems*, taught at the American University of
Beirut. The course teaches OS internals by progressively extending xv6:
students implement new system calls, processes, threading, scheduling,
and synchronization primitives directly inside the kernel.

The three phases were submitted as a team project. My contributions are
documented in the per-phase READMEs; the full code lives in this repo.

## Building (general)

Each phase folder uses the standard xv6 build system. From inside a phase
folder:

```bash
make qemu
```

You'll need a cross-compiler (`gcc-i686-linux-gnu` for x86 phases,
`riscv64-unknown-elf-gcc` or `riscv64-linux-gnu-gcc` for the RISC-V
phase) and QEMU. Detailed instructions and prerequisites are in each
phase's README.

## Reports

Team-written reports for each phase live in [`docs/`](./docs/), covering
design decisions, testing methodology, and known limitations.

## Upstream

- Phases 1 and 3 are based on MIT's original x86 xv6 (`xv6-public`).
- Phase 2 is based on the RISC-V port (`xv6-riscv`), which MIT has been
  maintaining since 2019.

The original xv6 license (MIT) applies to all upstream code. Original
extensions in this repo are released under the same license.