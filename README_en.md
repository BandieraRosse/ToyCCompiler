# ToyCCompiler (tcc)

> *“Yo sé quién soy.”*  
> — Miguel de Cervantes, *Don Quijote de la Mancha*

A self-bootstrapping C compiler for x86-64 Linux. Approximately 10,000 lines of C, zero external dependencies, zero libc — just a `syscall` instruction and the will to compile itself.

## What this is

ToyCCompiler is a small, standalone C compiler that can compile its own source code. It is not a fork of anything. It does not use LLVM, GCC, or any existing compiler infrastructure. It starts from a handful of C source files and ends with a running ELF binary — all without a single `#include <stdlib.h>`.

The runtime (`tcc_rt.c`) talks to Linux directly through inline `syscall` instructions. The output is statically linked ELF64 `.o` files. The test suite runs 28 self-hosted test programs that require no runtime library whatsoever.

## The bootstrap chain

```
gcc  ──→  stage-1 tcc
               │
               ↓  compile tcc source with itself
          stage-2 tcc
               │
               ↓  compile tcc source with itself
          stage-3 tcc
               │
               ↓  ...
          stage-4 tcc   ──→  byte-identical with 3, 5, 6, 7, 8, 9, 10
```

| Stage | Built by      | Size     | Selfhost tests | MD5                              |
|-------|---------------|----------|----------------|----------------------------------|
| 1     | gcc (host)    | 202,400  | 28/28 ✅       | (gcc-signed)                     |
| 2     | stage-1 tcc   | 349,544  | 28/28 ✅       | `f80002c7`                       |
| 3–10  | stage-2 tcc+  | 349,544  | 28/28 ✅       | `68de925f` (all identical)       |

Result: **Stage-3 through Stage-10 are byte-for-byte identical.** The compiler has reached its fixed point — a self-reproducing, closed loop of code that generates itself faithfully on every iteration.

The single 8-byte difference between Stage-2 and Stage-3 (in `lex.o`, two functions) is the fingerprint of the handoff: Stage-2 was compiled by GCC, Stage-3 by tcc itself. After that, the chain converges and stays there.

## What it looks like

```c
static long sys_write(int fd, const char *buf, unsigned long len) {
    unsigned long ret;
    __asm__ __volatile__ ("syscall"
        : "=a"(ret) : "a"(1), "D"((long)fd), "S"((long)buf), "d"((long)len)
        : "rcx", "r11", "memory");
    return (long)ret;
}

void __tlibc_start(void) {
    sys_write(1, "Hello, world.\n", 14);
    __asm__ ("syscall" : : "a"((long)60), "D"((long)0) : "rcx", "r11");
}
```

No `main()`. No `return`. No libc. The entry point is `__tlibc_start`, and the test programs exit via a raw `exit` syscall. The asm constraints are deliberately simple — just `"=a"` and `"a"` — because the compiler's asm support is minimal by design.

## Why this project exists

There is no shortage of C compilers. GCC is 15 million lines. LLVM is a sprawling ecosystem. Even Fabrice Bellard's `tcc` is ten times larger than this one.

This one exists for two reasons:

1. **To see if it could be done** — a self-bootstrapping C compiler in the fewest possible lines, with no dependencies, no runtime, no outside help. Just the source code, the CPU, and the System V ABI.

2. **To understand, completely** — a compiler you write yourself, you understand yourself. Every bug is yours. Every tradeoff is yours. Every `mov %rdi,-0x8(%rbp)` that finally comes out right is yours.

## The name

*ToyCCompiler* is honest about its limits. This compiler does not aspire to replace GCC or ship with a Linux distribution. It aspires to compile itself, prove its own consistency, and serve as a complete, readable artifact of how C becomes machine code.

The project's patron is Don Quixote — not because it fights windmills, but because it chooses a path that is clear-eyed, solitary, and its own.

## A note on the journey

The bug that blocked full self-bootstrapping was deceptively small: a `Lexer` struct copy in `parse_parameter_list` that only copied 8 bytes instead of 88. The effect was that `void *dst` arguments were silently truncated to 32 bits — `%edi` instead of `%rdi`. Stage-1 (compiled by GCC) had no problem. Stage-2 (compiled by Stage-1) inherited the bug silently. Stage-3 (compiled by Stage-2) crashed.

This is the classic *trusting trust* problem made visible: a compiler's bug can hide in its own self-compiled binary, invisible until the chain is tested by a third generation.

The fix was a single function. The verification was Stage-1 through Stage-10.

## Limits

- x86-64 Linux only
- C89 with some C99 features (no VLAs, no bitfields, no `_Generic`)
- No floating point in the default build
- `main()` return is broken — use `sys_exit()` instead
- Variadic calls limited to ~6 arguments
- Pointer arithmetic via `*(p + i)` is unreliable — use `p[i]`

For detailed build and test instructions, see [`CLAUDE.md`](./CLAUDE.md).

## Verification

```sh
make test           # 28/28  ✅
make test-selfhost  # 28/28  ✅
./bootstrap-to-10.sh  # stage-3 → stage-10 byte-identical ✅
```

All 28 self-host tests pass at every stage from Stage-1 through Stage-10. The compiler's output converges at Stage-3 and remains identical through Stage-10 — a complete, verified bootstrap.

---

*Written in 2026. Built with syscalls, entropy, and the quiet satisfaction of a 10,000-line compiler that compiles itself.*
