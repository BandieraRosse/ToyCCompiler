// EXPECT: 0
// 浮点：double 运算/比较/常量/强制转换/混合运算

static double add_dbl(double a, double b) { return a + b; }
static double mul_dbl(double a, double b) { return a * b; }

int main(void) {
    /* --- double 运算 --- */
    /* (from 018_double_arith) */
    {
        double x, y, z;
        x = 3.5;
        y = 2.0;
        z = x + y;
        if (z != 5.5) return 1;
        z = x - y;
        if (z != 1.5) return 2;
        z = x * y;
        if (z != 7.0) return 3;
        z = x / y;
        if (z != 1.75) return 4;
    }

    /* --- double 比较 --- */
    /* (from 019_double_cmp) */
    {
        double x = 3.5, y = 3.5, z = 4.0;
        if (!(x == y)) return 10;
        if (x == z)    return 11;
        if (!(x != z)) return 12;
        if (!(x < z))  return 13;
        if (!(z > x))  return 14;
        if (!(x <= y)) return 15;
        if (!(z >= y)) return 16;
    }

    /* --- 强制转换 double ↔ int --- */
    /* (from 024_cast) */
    {
        double d;
        int x;
        d = 3.14;
        x = (int)d;
        if (x != 3) return 20;
        x = (int)7.99;
        if (x != 7) return 21;
        d = (double)42;
        if (d != 42.0) return 22;
    }

    /* --- double 常量与函数调用 --- */
    /* (from 061_double_const, 062_double_func) */
    {
        double a = 3.14;
        if (a != 3.14) return 30;
        double b = 2.0e3;
        if (b != 2000.0) return 31;
        if (add_dbl(3.0, 4.0) != 7.0) return 32;
        if (mul_dbl(3.0, 4.0) != 12.0) return 33;
    }

    /* --- double 转 int --- */
    /* (from 063_double_to_int) */
    {
        int x;
        x = 3.14;
        if (x != 3) return 40;
        x = 7.99;
        if (x != 7) return 41;
        x = -3.9;
        if (x != -3) return 42;  /* 向零截断 */
    }

    /* --- int 与 double 混合运算 --- */
    /* (from 064_mixed_arith) */
    {
        double d;
        int i = 5;
        d = i + 3.0;
        if (d != 8.0) return 50;
        d = 10.5 - i;
        if (d != 5.5) return 51;
        d = i * 2.0;
        if (d != 10.0) return 52;
    }

    return 0;
}
