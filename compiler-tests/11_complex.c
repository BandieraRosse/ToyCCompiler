// EXPECT: 0
// 复杂功能：作用域、嵌套循环/if、复杂表达式、转义、十六进制、斐波那契

static int add2(int a, int b) { return a + b; }
static int fib(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

int main(void) {
    /* --- 块作用域与变量阴影 --- */
    /* (from 082_block_scope, 087_block_shadow) */
    {
        int x = 10;
        {
            int x = 20;
            if (x != 20) return 1;
        }
        if (x != 10) return 2;
        /* 多层嵌套阴影 */
        int a = 1;
        { int a = 2;
          { int a = 3;
            if (a != 3) return 3;
          }
          if (a != 2) return 4;
        }
        if (a != 1) return 5;
    }

    /* --- 变量初始化为表达式 --- */
    /* (from 083_var_init_expr) */
    {
        int x = 5;
        int y = x + 3;
        if (y != 8) return 10;
        int z = add2(3, 4);
        if (z != 7) return 11;
    }

    /* --- 嵌套循环 --- */
    /* (from 076_nested_loop) */
    {
        int sum = 0, i, j;
        for (i = 0; i < 3; i = i + 1)
            for (j = 0; j < 4; j = j + 1)
                sum = sum + 1;
        if (sum != 12) return 20;
        /* while 嵌套 for */
        sum = 0; i = 0;
        while (i < 3) {
            int k;
            for (k = 0; k < 4; k = k + 1)
                sum = sum + 1;
            i = i + 1;
        }
        if (sum != 12) return 21;
    }

    /* --- 嵌套 if --- */
    /* (from 077_nested_if) */
    {
        int result;
        int x = 1;
        if (x == 1)
            if (x == 2) result = 10;
            else result = 20;
        else result = 30;
        if (result != 20) return 30;
        /* 3 层嵌套 */
        x = 0;
        if (x == 0) {
            if (x == 0) {
                if (x == 0) result = 42;
                else result = 99;
            }
        }
        if (result != 42) return 31;
    }

    /* --- 复杂表达式 --- */
    /* (from 078_complex_expr) */
    {
        int x;
        x = (1 + 2) * 3 ^ 4 & 0xFF;
        if (x != 13) return 40;  /* 9 ^ 4 = 13 */
        x = (1 << 3) | 7;
        if (x != 15) return 41;
        x = 0xFF & ~(0x0F << 2);
        if (x != 0xCF) return 42;
    }

    /* --- 转义序列 --- */
    /* (from 079_escape_seq) */
    {
        if ('\0' != 0)  return 50;
        if ('\n' != 10) return 51;
        if ('\t' != 9)  return 52;
        if ('\r' != 13) return 53;
        if ('\\' != 92) return 54;
        if ('\'' != 39) return 55;
        if ('\"' != 34) return 56;
    }

    /* --- 十六进制字面量 --- */
    /* (from 080_hex_literal) */
    {
        if (0x0 != 0)       return 60;
        if (0x10 != 16)     return 61;
        if (0xFF != 255)    return 62;
        if (0x100 != 256)   return 63;
        if (0xABCD != 43981) return 64;
    }

    /* --- 斐波那契（递归+多级调用） --- */
    /* (from 085_fib) */
    if (fib(0) != 0) return 70;
    if (fib(1) != 1) return 71;
    if (fib(10) != 55) return 72;

    return 0;
}
