// EXPECT: 0
// SELF_CONTAINED
// 20_small_array.c — 小数组退化 + 小类型 brace-init 数据排放
//   1. unsigned char[4] 数组（≤8 字节）退化→指针，下标访问正确
//   2. unsigned short[5] brace-init 数据按 2 字节/slot 排放
//   3. int[2] 数组（恰 8 字节）退化→指针
//   4. char[3] 数组（<8 字节）退化→指针
//   5. 小数组传递到局部指针后再下标

static long sys_write(int fd, const char *buf, unsigned long len) {
    unsigned long ret;
    __asm__ __volatile__ ("syscall"
        : "=a"(ret) : "a"(1), "D"((long)fd), "S"((long)buf), "d"((long)len)
        : "rcx", "r11", "memory");
    return (long)ret;
}
static void sys_exit(int code) {
    __asm__ __volatile__ ("syscall"
        : : "a"((long)60), "D"((long)code)
        : "rcx", "r11", "memory");
    for (;;) ;
}

/* ─── 测试 1: unsigned char[4] ≤8 字节数组 ─── */
static unsigned char g_uc[4] = { 10, 20, 30, 40 };

/* ─── 测试 2: unsigned short[5] brace-init ─── */
static unsigned short g_us[5] = { 100, 200, 300, 400, 500 };

/* ─── 测试 3: int[2] 恰 8 字节数组 ─── */
static int g_i2[2] = { 1000, 2000 };

/* ─── 测试 4: char[3] <8 字节数组 ─── */
static char g_c3[3] = { 'X', 'Y', 'Z' };

/* ─── 测试 5: unsigned char[6] 传局部指针 ─── */
static unsigned char g_uc6[6] = { 5, 10, 15, 20, 25, 30 };

/* ─── 测试 9: int[1] 单元素（4 字节，gv_arr_len==1 曾被漏标记 is_array）─── */
static int g_i1[1] = { 999 };

/* ─── 测试 10: short[4] 恰 8 字节，元素 2 字节 ─── */
static short g_s4[4] = { 10, 20, 30, 40 };

/* ─── 测试 11: char[8] 恰 8 字节（type_size==8 曾被 heuristic 漏判）─── */
static char g_c8[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

void __tlibc_start(void)
{
    /* ═══ 测试 1: unsigned char[4] 下标 ═══ */
    {
        if (g_uc[0] != 10) { sys_write(1,"F1a\n",4); sys_exit(11); }
        if (g_uc[1] != 20) { sys_write(1,"F1b\n",4); sys_exit(12); }
        if (g_uc[2] != 30) { sys_write(1,"F1c\n",4); sys_exit(13); }
        if (g_uc[3] != 40) { sys_write(1,"F1d\n",4); sys_exit(14); }
    }

    /* ═══ 测试 2: unsigned short[5] brace-init ═══ */
    {
        if (g_us[0] != 100) { sys_write(1,"F2a\n",4); sys_exit(21); }
        if (g_us[1] != 200) { sys_write(1,"F2b\n",4); sys_exit(22); }
        if (g_us[2] != 300) { sys_write(1,"F2c\n",4); sys_exit(23); }
        if (g_us[3] != 400) { sys_write(1,"F2d\n",4); sys_exit(24); }
        if (g_us[4] != 500) { sys_write(1,"F2e\n",4); sys_exit(25); }
    }

    /* ═══ 测试 3: int[2] 恰 8 字节 ═══ */
    {
        if (g_i2[0] != 1000) { sys_write(1,"F3a\n",4); sys_exit(31); }
        if (g_i2[1] != 2000) { sys_write(1,"F3b\n",4); sys_exit(32); }
    }

    /* ═══ 测试 4: char[3] <8 字节 ═══ */
    {
        if (g_c3[0] != 'X') { sys_write(1,"F4a\n",4); sys_exit(41); }
        if (g_c3[1] != 'Y') { sys_write(1,"F4b\n",4); sys_exit(42); }
        if (g_c3[2] != 'Z') { sys_write(1,"F4c\n",4); sys_exit(43); }
    }

    /* ═══ 测试 5: 小数组传局部指针 ═══ */
    {
        unsigned char *p = g_uc6;
        if (p[0] != 5)  { sys_write(1,"F5a\n",4); sys_exit(51); }
        if (p[2] != 15) { sys_write(1,"F5b\n",4); sys_exit(52); }
        if (p[5] != 30) { sys_write(1,"F5c\n",4); sys_exit(53); }
    }

    /* ═══ 测试 6: int[2] 走局部指针后 *解引用 ═══ */
    {
        int *p = g_i2;
        if (*p != 1000)      { sys_write(1,"F6a\n",4); sys_exit(61); }
        if (p[1] != 2000)    { sys_write(1,"F6b\n",4); sys_exit(62); }
    }

    /* ═══ 测试 7: char[3] 走局部指针后 *解引用 ═══ */
    {
        char *p = g_c3;
        if (*p != 'X')       { sys_write(1,"F7a\n",4); sys_exit(71); }
        if (p[1] != 'Y')     { sys_write(1,"F7b\n",4); sys_exit(72); }
        if (p[2] != 'Z')     { sys_write(1,"F7c\n",4); sys_exit(73); }
    }

    /* ═══ 测试 8: unsigned char[4] 走局部指针 ═══ */
    {
        unsigned char *p = g_uc;
        if (*p != 10)        { sys_write(1,"F8a\n",4); sys_exit(81); }
        if (p[1] != 20)      { sys_write(1,"F8b\n",4); sys_exit(82); }
        if (p[2] != 30)      { sys_write(1,"F8c\n",4); sys_exit(83); }
        if (p[3] != 40)      { sys_write(1,"F8d\n",4); sys_exit(84); }
    }

    /* ═══ 测试 9: int[1] 单元素数组退化指针 ═══ */
    {
        int *p = g_i1;
        if (*p != 999)       { sys_write(1,"F9a\n",4); sys_exit(91); }
        if (p[0] != 999)     { sys_write(1,"F9b\n",4); sys_exit(92); }
    }

    /* ═══ 测试 10: short[4] 8 字节退化指针 ═══ */
    {
        short *p = g_s4;
        if (*p != 10)        { sys_write(1,"F10a\n",4); sys_exit(101); }
        if (p[1] != 20)      { sys_write(1,"F10b\n",4); sys_exit(102); }
        if (p[3] != 40)      { sys_write(1,"F10c\n",4); sys_exit(103); }
    }

    /* ═══ 测试 11: char[8] 恰 8 字节退化指针 ═══ */
    {
        char *p = g_c8;
        if (*p != 'a')       { sys_write(1,"F11a\n",4); sys_exit(111); }
        if (p[4] != 'e')     { sys_write(1,"F11b\n",4); sys_exit(112); }
        if (p[7] != 'h')     { sys_write(1,"F11c\n",4); sys_exit(113); }
    }

    /* ═══ 测试 12: 局部数组退化指针（大小恰 8 字节） ═══ */
    {
        int local_arr[2] = { 42, 43 };
        int *p = local_arr;
        if (*p != 42)        { sys_write(1,"F12a\n",4); sys_exit(121); }
        if (p[1] != 43)      { sys_write(1,"F12b\n",4); sys_exit(122); }
    }

    /* ═══ 测试 13: 局部 char[8] 退化指针 ═══ */
    {
        char local_c8[8] = { 'p', 'q', 'r', 's', 't', 'u', 'v', 'w' };
        char *p = local_c8;
        if (*p != 'p')       { sys_write(1,"F13a\n",4); sys_exit(131); }
        if (p[7] != 'w')     { sys_write(1,"F13b\n",4); sys_exit(132); }
    }

    /* ═══ 测试 14: *(p+1) 指针运算解引用（int*，4 字节） ═══ */
    {
        int *p = g_i2;
        if (*(p+0) != 1000)  { sys_write(1,"F14a\n",4); sys_exit(141); }
        if (*(p+1) != 2000)  { sys_write(1,"F14b\n",4); sys_exit(142); }
    }

    /* ═══ 测试 15: *(p+1) 指针运算解引用（char*，1 字节） ═══ */
    {
        char *p = g_c3;
        if (*(p+0) != 'X')   { sys_write(1,"F15a\n",4); sys_exit(151); }
        if (*(p+1) != 'Y')   { sys_write(1,"F15b\n",4); sys_exit(152); }
        if (*(p+2) != 'Z')   { sys_write(1,"F15c\n",4); sys_exit(153); }
    }

    /* ═══ 测试 16: *(p+1) 指针运算解引用（short*，2 字节） ═══ */
    {
        short *p = g_s4;
        if (*(p+0) != 10)    { sys_write(1,"F16a\n",4); sys_exit(161); }
        if (*(p+1) != 20)    { sys_write(1,"F16b\n",4); sys_exit(162); }
        if (*(p+3) != 40)    { sys_write(1,"F16c\n",4); sys_exit(163); }
    }

    sys_write(1, "OK\n", 3);
    sys_exit(0);
}
