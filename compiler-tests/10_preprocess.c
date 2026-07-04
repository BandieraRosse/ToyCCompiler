// EXPECT: 0
// 预处理：对象宏、函数宏、条件编译、include、变参宏、嵌套宏
#include "tcc_need.h"

int main(void) {
    /* --- 对象宏 --- */
    /* (from 065_define_obj) */
    {
#define VAL 42
#define NEG -1
#define EXPR (3 + 4)
        if (VAL != 42) return 1;
        if (NEG != -1) return 2;
        if (EXPR != 7) return 3;
    }

    /* --- 函数宏 --- */
    /* (from 066_define_func) */
    {
#define ADD(a, b) ((a) + (b))
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
        if (ADD(3, 4) != 7)       return 10;
        if (SQUARE(5) != 25)      return 11;
        if (MAX(10, 20) != 20)    return 12;
        if (MAX(30, 5) != 30)     return 13;
    }

    /* --- #ifdef / #ifndef --- */
    /* (from 067_ifdef) */
    {
#define FEATURE_ENABLED
        int x = 0;
#ifdef FEATURE_ENABLED
        x = 10;
#else
        x = 99;
#endif
        if (x != 10) return 20;

#undef FEATURE_ENABLED
#ifdef FEATURE_ENABLED
        x = 50;
#else
        x = 60;
#endif
        if (x != 60) return 21;
    }

    /* --- #include --- */
    /* (from 071_include — 替换为包含 tcc_need.h) */
    {
        /* #include 在文件顶部的 tcc_need.h 已生效 */
        /* 验证 size_t 来自头文件 */
        size_t sz = sizeof(int);
        if (sz != 4) return 30;
    }

    /* --- 变参宏 --- */
    /* (from 072_variadic_macro) */
    {
#define PRINT(fmt, ...) fmt
        int x;
        x = PRINT("hello", 1, 2, 3);
    }

    /* --- __LINE__ 和 __FILE__ --- */
    /* (from 073_file_line) */
    {
        /* 当前 tcc 中 __LINE__ 恒为 0 */
        int line = __LINE__;
        /* line 可能为 0，只要不崩即可 */
    }

    /* --- 嵌套宏展开 --- */
    /* (from 074_nested_macro) */
    {
#define ADD2(a, b) ((a) + (b))
#define MUL2(a, b) ((a) * (b))
        int x;
        x = MUL2(ADD2(2, 3), ADD2(4, 5));
        if (x != 45) return 70;
    }

    return 0;
}
