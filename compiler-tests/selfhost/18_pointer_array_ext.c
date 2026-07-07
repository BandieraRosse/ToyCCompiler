// EXPECT: 0
// SELF_CONTAINED
// pointer_array_ext.c — 扩展指针数组测试：
//   1. 全局 char * 双重下标（跨边界 + 变量下标）
//   2. 全局 int * 双重下标（file-scope 指针数组 int *ptrs[i][j]）
//   3. 局部指针数组 *ptrs[0] 解引用

static long sys_write(int fd, const char *buf, unsigned long len)
{
    unsigned long ret;
    __asm__ __volatile__ ("syscall"
        : "=a"(ret) : "a"(1), "D"((long)fd), "S"((long)buf), "d"((long)len)
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

/* ─── 测试 2 用的全局数据 ─── */
static int g_arr0[3] = { 10, 20, 30 };
static int g_arr1[3] = { 40, 50, 60 };
static int g_arr2[3] = { 70, 80, 90 };
static int *g_int_ptrs[3];

void __tlibc_start(void)
{
    /* ═══ 测试 1: char * 双重下标（跨边界 + 变量下标） ═══ */
    {
        static const char *strs[4] = { "AB", "CDE", "FGHI", "JK" };
        if (strs[0][0] != 'A') { sys_write(1,"F1a\n",4); sys_exit(1); }
        if (strs[0][1] != 'B') { sys_write(1,"F1b\n",4); sys_exit(1); }
        if (strs[1][2] != 'E') { sys_write(1,"F1c\n",4); sys_exit(1); }
        if (strs[2][3] != 'I') { sys_write(1,"F1d\n",4); sys_exit(1); }
        if (strs[3][0] != 'J') { sys_write(1,"F1e\n",4); sys_exit(1); }
        if (strs[3][1] != 'K') { sys_write(1,"F1f\n",4); sys_exit(1); }
        /* 变量下标 */
        {
            int i, j;
            const char *all = "ABCDEFGHIJK";
            for (i = 0; i < 4; i++) {
                int maxj = (i==0)?2:(i==1)?3:(i==2)?4:2;
                for (j = 0; j < maxj; j++) {
                    if (strs[i][j] != all[j + (i==0?0:(i==1?2:(i==2?5:9)))]) {
                        sys_write(1,"F1v\n",4); sys_exit(1);
                    }
                }
            }
        }
    }

    /* ═══ 测试 2: int * 全局指针数组双重下标 ═══ */
    g_int_ptrs[0] = g_arr0;
    g_int_ptrs[1] = g_arr1;
    g_int_ptrs[2] = g_arr2;
    if (g_int_ptrs[0][0] != 10) { sys_write(1,"F2a\n",4); sys_exit(1); }
    if (g_int_ptrs[0][2] != 30) { sys_write(1,"F2b\n",4); sys_exit(1); }
    if (g_int_ptrs[1][1] != 50) { sys_write(1,"F2c\n",4); sys_exit(1); }
    if (g_int_ptrs[2][0] != 70) { sys_write(1,"F2d\n",4); sys_exit(1); }
    if (g_int_ptrs[2][2] != 90) { sys_write(1,"F2e\n",4); sys_exit(1); }
    /* 变量下标 */
    {
        int i, j;
        for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++) {
                int exp = (i==0?10:i==1?40:70) + j*10;
                if (g_int_ptrs[i][j] != exp) { sys_write(1,"F2v\n",4); sys_exit(1); }
            }
    }

    /* ═══ 测试 3: *ptrs[0] 解引用（局部指针数组） ═══ */
    {
        int a = 100, b = 200, c = 300;
        int *local_ptrs[3];
        local_ptrs[0] = &a;
        local_ptrs[1] = &b;
        local_ptrs[2] = &c;
        if (*local_ptrs[0] != 100) { sys_write(1,"F3a\n",4); sys_exit(1); }
        if (*local_ptrs[1] != 200) { sys_write(1,"F3b\n",4); sys_exit(1); }
        if (*local_ptrs[2] != 300) { sys_write(1,"F3c\n",4); sys_exit(1); }
    }

    sys_write(1, "OK\n", 3);
    sys_exit(0);
}
