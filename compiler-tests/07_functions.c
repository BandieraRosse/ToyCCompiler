// EXPECT: 0
// 函数：参数传递（0-8个）、函数指针、递归、嵌套调用

static int get_val(void) { return 42; }
static int dbl(int x) { return x * 2; }
static int sum6(int a, int b, int c, int d, int e, int f) {
    return a + b + c + d + e + f;
}
static int sum7(int a, int b, int c, int d, int e, int f, int g) {
    return a + b + c + d + e + f + g;
}
static int sum8(int a, int b, int c, int d, int e, int f, int g, int h) {
    return a + b + c + d + e + f + g + h;
}
static int add(int a, int b) { return a + b; }
static int mul(int a, int b) { return a * b; }
static int sub(int a, int b) { return a - b; }
static int inc(int x) { return x + 1; }
static int dec(int x) { return x - 1; }
static int twice(int x) { return x * 2; }
static int fact(int n) { if (n <= 1) return 1; return n * fact(n - 1); }

int main(void) {
    /* --- 0 参数函数 --- */
    /* (from 038_func_call_0) */
    if (get_val() != 42) return 1;

    /* --- 1 参数函数 --- */
    /* (from 039_func_call_1) */
    if (dbl(5) != 10) return 10;
    if (dbl(21) != 42) return 11;

    /* --- 6 参数函数 --- */
    /* (from 040_func_call_6) */
    if (sum6(1, 2, 3, 4, 5, 6) != 21) return 20;

    /* --- 7-8 参数函数 --- */
    /* (from 041_func_call_7) */
    if (sum7(1, 2, 3, 4, 5, 6, 7) != 28) return 30;
    if (sum8(1, 2, 3, 4, 5, 6, 7, 8) != 36) return 31;

    /* --- 函数调用作表达式 --- */
    /* (from 042_func_call_expr) */
    if (add(mul(2, 3), mul(4, 5)) != 26) return 40;

    /* --- 函数指针 --- */
    /* (from 043_func_ptr) */
    {
        int (*fp)(int, int);
        fp = add;
        if (fp(10, 3) != 13) return 50;
        fp = sub;
        if (fp(10, 3) != 7)  return 51;
    }

    /* --- 递归 --- */
    /* (from 044_recursion) */
    if (fact(5) != 120) return 60;
    if (fact(0) != 1) return 61;

    /* --- 嵌套调用 --- */
    /* (from 045_nested_call) */
    if (inc(dec(10)) != 10)  return 70;
    if (twice(inc(5)) != 12) return 71;
    if (inc(twice(dec(10))) != 19) return 72;

    return 0;
}
