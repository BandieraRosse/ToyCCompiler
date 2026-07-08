// EXPECT: 0
// t01_struct_return_expr.c — Bug A: struct-return + direct member access
//
// tcc bug: lexer_next(&lx).kind reads garbage when accessing .kind directly
//          on struct returned by function (hidden pointer ABI issue).
// Workaround: Token t = lexer_next(&lx); t.kind — works fine.

static void sys_exit(int code) {
    __asm__ __volatile__ ("syscall"
        : : "a"((long)60), "D"((long)code)
        : "rcx", "r11", "memory");
    for (;;) ;
}

struct BigToken {
    int kind;
    long pad[5];  // 40 bytes padding → total 48 bytes, forces hidden ptr
};

static struct BigToken make_tok(int k) {
    struct BigToken t;
    t.kind = k;
    t.pad[0] = 1; t.pad[1] = 2; t.pad[2] = 3;
    t.pad[3] = 4; t.pad[4] = 5;
    return t;
}

void __tlibc_start(void) {
    /* Test A: direct member access on returned struct (tcc FAILS) */
    if (make_tok(42).kind != 42) sys_exit(1);

    /* Test B: store to local first, then access (tcc OK) */
    struct BigToken t = make_tok(99);
    if (t.kind != 99) sys_exit(2);
    if (t.pad[4] != 5) sys_exit(3);

    /* Test C: multiple calls with direct member access */
    if (make_tok(77).kind != 77) sys_exit(4);

    /* Note: func().non_first_member (like .pad[0]) is not tested here
     * because tcc lacks type tracking for function return values. The
     * struct member offset can't be determined without knowing the return
     * type at parse time. This pattern is not used in compiler sources. */

    sys_exit(0);
}
