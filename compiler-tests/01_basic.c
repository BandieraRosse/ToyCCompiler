// EXPECT: 0
// 基础：空函数、返回常量、局部变量、算术运算
int main(void) {
    /* --- 空 main / return 常量 --- */
    /* (from 001_empty, 002_return_int) */

    /* --- 局部变量定义与赋值 --- */
    /* (from 003_local_var) */
    {
        int a;
        a = 10;
        if (a != 10) return 1;
        int b = 20;
        if (b != 20) return 2;
        int c = 30, d = 40;
        if (c != 30) return 3;
        if (d != 40) return 4;
    }

    /* --- 算术运算 + - * / % --- */
    /* (from 004_arith) */
    {
        if (2 + 3 != 5)  return 10;
        if (7 - 4 != 3)  return 11;
        if (3 * 4 != 12) return 12;
        if (13 / 5 != 2) return 13;
        if (13 % 5 != 3) return 14;
        if (2 + 3 * 4 != 14) return 15;
        if ((2 + 3) * 4 != 20) return 16;
    }

    return 0;
}
