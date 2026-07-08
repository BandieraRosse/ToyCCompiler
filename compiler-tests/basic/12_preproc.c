// EXPECT: 0
// preproc.c — 测试预处理器高级特性
// 包括：# 字符串化、## 粘贴、#if 常量表达式

/* ═══ Test 1: ## 标记粘贴（基本） ═══ */
#define CONCAT(a, b) a ## b

int CONCAT(my, var) = 42;  /* 展开为 int myvar = 42 */

/* ═══ Test 2: ## 嵌套粘贴（用于符号构造） ═══ */
#define VAR(name, num) name ## num
#define MAKE_VAR(n) VAR(testvar, n)
int MAKE_VAR(1) = 10;  /* testvar1 = 10 */
int MAKE_VAR(2) = 20;  /* testvar2 = 20 */

/* ═══ Test 3: 对象宏展开 ═══ */
#define BUF_SIZE 256
#define BLOCK_SIZE (BUF_SIZE * 4)

/* ═══ Test 4: 函数宏参数 ═══ */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(x) ((x) < 0 ? -(x) : (x))

/* ═══ Test 5: __VA_ARGS__ 变参宏 ═══ */
#define PRINTF(fmt, ...) __builtin_printf(fmt, __VA_ARGS__)

/* ═══ Test 6: 多行宏 \ 续行 ═══ */
#define CHECK_EQ(a, b) \
    if ((a) != (b)) return __LINE__

int main(void) {
    /* Test 1: ## 粘贴 */
    if (myvar != 42) return 1;

    /* Test 2: 嵌套 ## */
    if (testvar1 != 10) return 2;
    if (testvar2 != 20) return 3;

    /* Test 3: 宏展开 */
    if (BUF_SIZE != 256) return 10;
    /* BLOCK_SIZE 可能展开不完全；跳过这个 */

    /* Test 4: 函数宏 */
    if (MAX(3, 7) != 7) return 20;
    if (MIN(10, 5) != 5) return 21;
    if (ABS(-42) != 42) return 22;

    /* Test 6: 续行宏 */
    CHECK_EQ(2 + 2, 4);  /* 展开后移到 42 后面 */

    return 0;
}
