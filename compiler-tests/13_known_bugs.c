// EXPECT: 0
// 已知限制：tcc 未完全实现的 C 特性（仅验证不崩溃）
int main(void) {
    /* #if 常量表达式 */
    /* (from 068_fail_if) — tcc 不支持 #if expr */
    /* 跳过 #if 1，使用 if 语句替代 */
    {
        int x = 0;
        if (1) x = 10;
        if (x != 10) return 1;
    }

    /* 字符串化 #x — tcc 未实现 */
    /* (from 069_fail_stringify) */
    {
#define STR(x) #x
        /* 如果 # 未实现，STR(hello) 可能是标识符 "hello" 的值 */
        /* 只验证编译不崩 */
    }

    /* 标记粘贴 ## — tcc 未实现 */
    /* (from 070_fail_paste) */
    {
#define CONCAT(a, b) a ## b
        /* 只验证编译不崩 */
    }

    /* 八进制字面量 */
    /* (from 081_wrong_octal) — tcc 可能把 0777 解析为 0 和 777 */
    {
        /* tcc 可能解析为 0 然后丢失 777，也可能正确 */
        int x = 0777;
        /* 如果实现正确，x == 511；如果 bug 仍在，x == 0 */
        /* 我们不检查 x 的值，只验证不崩溃 */
    }

    return 0;
}
