// EXPECT: 0
// SELF_CONTAINED
// negativelong.c — 测试并验证 tcc long 负数处理 BUG 及绕过方法
//
// tcc 的 BUG：加载 32 位值到寄存器使用 mov eax（零扩展）而非 movsx（符号扩展），
// 导致 int→long 的编译期隐式转换丢失符号。
//    long v = -12345;  // 得到 4294954951 (0x00000000FFFFCFC7) 而非 -12345
//    long v = -12345L; // 同上（编译期计算）
//
// 绕过方法：通过运行期间接求反或减法得到负值。
//    long pos = 12345; long neg = -pos;         // ✓ 正确
//    long neg = 0; neg = neg - 12345;             // ✓ 正确（运行期减法）

// ============================================================
// syscall 包装（约束驱动，不依赖外部 .o）
// ============================================================

static long sys_write(int fd, const char *buf, unsigned long len)
{
    unsigned long ret;
    __asm__ __volatile__ ("syscall"
        : "=a"(ret)
        : "a"(1), "D"((long)fd), "S"((long)buf), "d"((long)len)
        : "rcx", "r11", "memory");
    return (long)ret;
}

static void sys_exit(int code)
{
    __asm__ __volatile__ ("syscall"
        : : "a"((long)60), "D"((long)code)
        : "rcx", "r11", "memory");
    for (;;) ;
}

// ============================================================
// my_printf 实现
// ============================================================

static int my_strlen(const char *s) {
    int n = 0; while (*s) { s++; n++; } return n;
}

static void print_dec(long n) {
    char buf[32]; int i = 0;
    if (n < 0) { sys_write(1, "-", 1); n = -n; }
    if (n == 0) { sys_write(1, "0", 1); return; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (i > 0) sys_write(1, &buf[--i], 1);
}

static void print_hex(unsigned long n) {
    const char *hex = "0123456789abcdef";
    char buf[17]; int i = 0;
    if (n == 0) { sys_write(1, "0", 1); return; }
    while (n > 0) { buf[i++] = hex[n & 0xf]; n >>= 4; }
    while (i > 0) sys_write(1, &buf[--i], 1);
}

static void my_printf(const char *fmt, ...) {
    __builtin_va_list ap;
    __builtin_va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') { sys_write(1, fmt, 1); fmt++; continue; }
        fmt++;
        switch (*fmt) {
        case 's': {
            const char *s = __builtin_va_arg(ap, const char *);
            sys_write(1, s, (unsigned long)my_strlen(s)); break;
        }
        case 'd': case 'i':
            print_dec(__builtin_va_arg(ap, long)); break;
        case 'c': {
            char c = (char)__builtin_va_arg(ap, int);
            sys_write(1, &c, 1); break;
        }
        case 'x': case 'X':
            print_hex(__builtin_va_arg(ap, unsigned long)); break;
        case '%': sys_write(1, "%", 1); break;
        default:  sys_write(1, fmt, 1); break;
        }
        fmt++;
    }
    __builtin_va_end(ap);
}

// ============================================================
// main — 测试 long 负值在不同初始化模式下的表现
// ============================================================

int main(void)
{
    my_printf("=== tcc long negative number test ===\n\n");

    /* ─── 测试 1：编译期直接赋值（BUG 模式，仅输出，不断言） ─── */
    long v1 = -12345;          /* 编译期 int → long，BUG：零扩展 */
    long v2 = -12345L;         /* 编译期 long literal，同 BUG */
    long v3 = 0 - 12345;       /* 编译期 int 减法，同 BUG */
    long v4 = 0 - 12345L;      /* 编译期 long 减法，同 BUG */

    my_printf("compile-time int -12345:     %d  (0x%x)\n", v1, v1);
    my_printf("compile-time -12345L:         %d  (0x%x)\n", v2, v2);
    my_printf("compile-time 0-12345:         %d  (0x%x)\n", v3, v3);
    my_printf("compile-time 0-12345L:        %d  (0x%x)\n", v4, v4);

    /* ─── 测试 2：运行期计算（绕过方法，应正确） ─── */
    long tmp1 = 12345;
    long v5 = -tmp1;           /* 运行期 long 求反 ✓ */
    long tmp2 = 0L;
    long v6 = tmp2 - 12345L;   /* 运行期 long 减法 ✓ */
    long tmp3 = 12345;
    long v7 = 0;
    v7 = v7 - tmp3;            /* 运行期复合减法 ✓ */

    my_printf("runtime -(long)12345:        %d  (0x%x)\n", v5, v5);
    my_printf("runtime 0L-12345L:           %d  (0x%x)\n", v6, v6);
    my_printf("runtime 0-tmp:               %d  (0x%x)\n", v7, v7);

    /* 验证运行期方法结果正确（比较双方都须运行时计算，避免 tcc BUG） */
    {
        long expected = 12345; expected = -expected;
        if (v5 != expected) return 10;
        if (v6 != expected) return 11;
        if (v7 != expected) return 12;
    }

    /* ─── 测试 3：long 参数传入 my_printf 的变参路径 ─── */
    {
        /* 运行时计算负值传入 %d（%d 读 long，须传 long） */
        long neg1 = 42;    neg1 = -neg1;
        long neg2 = 99;    neg2 = -neg2;
        long bigr = 2000000000L;
        bigr = -bigr;

        my_printf("\nvariadic %%d with runtime neg:\n");
        my_printf("  neg1=%d neg2=%d big=%d\n", neg1, neg2, bigr);

        /* 比较双方都须运行时计算 */
        {
            long e1 = 42; e1 = -e1;
            long e2 = 99; e2 = -e2;
            long e3 = 2000000000L; e3 = -e3;
            if (neg1 != e1) return 20;
            if (neg2 != e2) return 21;
            if (bigr != e3) return 22;
        }
    }

    /* ─── 测试 4：正数边界值（不触发 BUG，应始终正确） ─── */
    {
        long pos1 = 0;
        long pos2 = 2147483647;   /* INT_MAX */
        long pos3 = 4294967295L;  /* 超过 int 范围，走 mov_rax_imm64 路径 */
        long pos4 = 1234567890123L;

        my_printf("\npositive long values:\n");
        my_printf("  0=%d maxint=%d big32=%d big64=%d\n",
            pos1, pos2, pos3, pos4);

        if (pos1 != 0L) return 30;
        if (pos2 != 2147483647L) return 31;
        if (pos3 != 4294967295L) return 32;
        if (pos4 != 1234567890123L) return 33;
    }

    my_printf("\n=== ALL NEGATIVE LONG TESTS PASSED ===\n");
    return 0;
}

void __tlibc_start(void) { sys_exit(main()); }
