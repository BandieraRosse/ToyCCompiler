// EXPECT: 0
// goto_label.c — 测试 goto 和标签
// 自举关键：cgen.c 使用 goto 模式

int main(void) {
    int a = 0;

    /* 1. goto 和标签 */
    goto label1;
    a = 1;  /* 跳过 */
    return 1;

label1:
    if (a != 0) return 2;

    /* 2. 向后 goto（循环模拟） */
    a = 0;
label2:
    a = a + 1;
    if (a < 5) goto label2;
    if (a != 5) return 10;

    /* 3. 跳过初始化 */
    goto label3;
    {
        int unused = 0;
        return 11;
    }
label3:
    if (a != 5) return 12;

    /* 4. 多个标签 */
label4:
label5:
    a = 99;
    if (a != 99) return 20;

    return 0;
}
