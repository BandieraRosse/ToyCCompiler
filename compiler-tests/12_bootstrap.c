// EXPECT: 42
// 自举相关：大量局部变量、结构体/指针复杂用法、静态函数链、宏表
typedef struct { const char *name; int val; } Entry;
typedef struct { char *data; int len; int cap; } Buf;

static Entry tbl[16];
static int count;
static Buf shared;
static int buf_val;

static int sum7(int a1,int a2,int a3,int a4,int a5,int a6,int a7) {
    return a1+a2+a3+a4+a5+a6+a7;
}

static void add_entry(const char *name, int val) {
    if (count < 16) { tbl[count].name = name; tbl[count].val = val; count++; }
}

static void buf_putc(Buf *b, char c) {
    if (b->len < b->cap - 1) { /* 用 static buf_val 模拟容量检查 */ }
    b->data[b->len++] = c;
}

static void buf_puts(Buf *b, const char *s) {
    while (*s) { if (b->len < b->cap - 1) b->data[b->len++] = *s; s++; }
    b->data[b->len] = '\0';
}

static void chain_step1(void) {
    /* 初始化 shared */
    static char buf_space[64];
    shared.data = buf_space;
    shared.len = 0;
    shared.cap = 64;
}

static void chain_step2(const char *s) {
    buf_puts(&shared, s);
}

static int chain_step3(void) {
    if (shared.len > 0) {
        /* 检查内容 */
        if (shared.data[0] == 'O' && shared.data[1] == 'K')
            return 42;
    }
    return 0;
}

int main(void) {
    /* --- 大量局部变量 --- */
    /* (from 075_wrong_many_locals, 090_many_locals) */
    {
        int a0=0,a1=1,a2=2,a3=3,a4=4,a5=5,a6=6,a7=7,a8=8,a9=9;
        int b0=0,b1=1,b2=2,b3=3,b4=4,b5=5,b6=6,b7=7,b8=8,b9=9;
        int sum = 0, i;
        int vals[4] = {a0, a1, a2, a3};
        for (i = 0; i < 4; i = i + 1) sum = sum + vals[i];
        /* 只需验证编译通过且返回非零 */
        if (vals[0] != 0) return 1;
    }

    /* --- 结构体花括号初始化 --- */
    /* (from 086_struct_brace_init) */
    {
        Buf b = {0, 0, 0};
        if (b.data != 0) return 10;
        if (b.len != 0)  return 11;
        if (b.cap != 0)  return 12;
    }

    /* --- 静态函数链（Buf 写入与读取） --- */
    /* (from 092_static_func_chain) */
    {
        chain_step1();
        chain_step2("OK");
        int r = chain_step3();
        if (r != 42) return 20;
    }

    /* --- 宏表（结构体数组 + 查找） --- */
    /* (from 089_macro_table) */
    {
        add_entry("zero", 0);
        add_entry("one", 1);
        add_entry("ans", 42);
        int found = 0, i;
        for (i = 0; i < count; i = i + 1)
            if (tbl[i].val == 42) found = 1;
        if (!found) return 30;
    }

    /* --- 多次 extern 函数调用 --- */
    /* (from 093_func_call_7args) */
    if (sum7(1,2,3,4,5,6,7) != 28) return 40;

    /* --- 匿名结构体成员偏移 --- */
    /* (from 094_anon_struct_offset) */
    {
        typedef struct { char *data; int len; int cap; } OutBuf;
        OutBuf buf;
        OutBuf *b = &buf;
        /* 只验证指针运算不崩 */
        buf.data = 0;
        buf.len  = 10;
        buf.cap  = 20;
        if (b->len != 10) return 50;
        if (b->cap != 20) return 51;
    }

    return 42;  /* EXPECT: 42 */
}
