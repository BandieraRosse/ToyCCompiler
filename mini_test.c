// EXPECT: 0
// Minimal test: -> member access through struct chain to a pointer member
typedef struct { const char *x; int y; } Inner;
typedef struct { Inner inner; int z; } Outer;

static long sys_exit(int code) {
    long ret;
    __asm__ __volatile__ ("syscall" : "=a"(ret) : "a"((long)60), "D"((long)code) : "rcx", "r11", "memory");
    for (;;) ;
    return ret;
}

void __tlibc_start(void) {
    Outer o;
    o.inner.x = "hello";
    Outer *p = &o;
    const char *t = p->inner.x;  // THIS IS THE KEY LINE
    const char *expected = "hello";
    // Verify t points to correct string (if possible)
    if (t && expected) {
        sys_exit(0);  // pass
    }
    sys_exit(1);  // fail
}
