/*
 * 33_struct_array_sizeof.c — struct 数组遍历与 2D 数组访问验证
 *
 * 场景：
 *   1. struct 数组（InstrDef instrs[]）遍历时，find_instr_by_fmt
 *      能通过 arr[i].name / arr[i].fmt 正确定位目标元素
 *   2. 2D char/ int 数组的 fill + verify，验证步长计算和字符加载正确
 *
 * 历史：
 *   - struct 数组成员地址计算（AST_MEMBER 的数组成员退化指针）曾在 cgen_expr.c 缺失
 *   - 2D 数组 char 加载曾错误使用 movsbl（符号扩展）而非 movzbl（unsigned 时）
 *   - (char) 转型在比较时未做值截断，≥128 的值导致比较失败
 *   - main() 入口的花括号初始化数组重写入（mov %eax, [instrs]）破坏数据段
 *   以上均以修复。详见 SESSION_SUMMARY 2026-07-09。
 *
 * EXPECT: 0
 */

/* ─── struct 数组 sizeof bug ─── */

typedef struct {
    const char *name;
    int fmt;
    unsigned char opcode[2];
    int oplen;
    int op_ext;
} InstrDef;

static const InstrDef instrs[] = {
    {"mov", 1, {0x89},1,0},
    {"mov", 2, {0xC7},1,0},
    {"mov", 3, {0xB0},1,0},
    {"lea", 4, {0x8D},1,0},
    {"ret", 5, {0xC3},1,0},
    {0, 0, {0},0,0}
};

static int find_instr_by_fmt(const char *name, int want_fmt) {
    int i;
    for (i = 0; instrs[i].name; i++) {
        if (instrs[i].name[0] == name[0] && instrs[i].fmt == want_fmt)
            return 1;
    }
    return 0;
}

/* ─── 2D 数组 stride bug ─── */

static char sym_names[4][16];
static int  int_matrix[4][8];

static void fill_names(void) {
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 8; j++) {
            sym_names[i][j] = (char)('a' + i * 8 + j);
            int_matrix[i][j] = i * 8 + j;
        }
}

static int check_names(void) {
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 8; j++) {
            if (sym_names[i][j] != (char)('a' + i * 8 + j))
                return i * 100 + j + 1;
            if (int_matrix[i][j] != i * 8 + j)
                return 200 + i * 100 + j + 1;
        }
    return 0;
}

int main(void) {
    /* 测试 struct 数组遍历 */
    if (!find_instr_by_fmt("mov", 2)) return 10;

    /* 测试 2D 数组 */
    fill_names();
    {
        int r = check_names();
        if (r != 0) return r;
    }

    return 0;
}

void __tlibc_start(void) {
    __asm__ __volatile__ ("syscall" : : "a"((long)60), "D"((long)main()) : "rcx", "r11");
}
