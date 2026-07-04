// EXPECT: 0
// 类型：char/short/long/unsigned/sizeof、自增自减、复合赋值、链式赋值
int main(void) {
    /* --- char --- */
    /* (from 014_char) */
    {
        char c;
        c = 'A';
        if (c != 65) return 1;
        c = 100;
        if (c != 100) return 2;
        /* char 溢出取决于实现（signed/unsigned），跳过边界验证 */
    }

    /* --- short --- */
    /* (from 015_short) */
    {
        short s;
        s = 1000;
        if (s != 1000) return 10;
        s = 32000;
        if (s != 32000) return 11;
    }

    /* --- long --- */
    /* (from 016_long) */
    {
        long x;
        x = 100000;
        if (x != 100000) return 20;
        x = 2000000000L;
        if (x != 2000000000L) return 21;
    }

    /* --- unsigned --- */
    /* (from 017_unsigned) */
    {
        unsigned int u;
        u = 100;
        if (u != 100) return 30;
        u = 0xFFFFFFFF;
        if (u != 4294967295U) return 31;
    }

    /* --- sizeof --- */
    /* (from 022_sizeof_type, 023_sizeof_expr) */
    {
        if (sizeof(int)    != 4) return 40;
        if (sizeof(char)   != 1) return 41;
        if (sizeof(short)  != 2) return 42;
        if (sizeof(long)   != 8) return 43;
        if (sizeof(double) != 8) return 44;
        int x;
        double d;
        int arr[10];
        if (sizeof(x)       != 4) return 45;
        if (sizeof(d)       != 8) return 46;
        if (sizeof(arr)     != 40) return 47;
        if (sizeof(arr[0])  != 4) return 48;
    }

    /* --- 前缀自增/自减 ++x --x --- */
    /* (from 025_prefix_inc) */
    {
        int x = 5, y;
        y = ++x;
        if (x != 6) return 50;
        if (y != 6) return 51;
        y = --x;
        if (x != 5) return 52;
        if (y != 5) return 53;
    }

    /* --- 后缀自增/自减 x++ x-- --- */
    /* (from 026_wrong_postfix_inc) */
    {
        int x = 5, a;
        a = x++;
        if (a != 5) return 60;
        if (x != 6) return 61;
        a = x--;
        if (a != 6) return 62;
        if (x != 5) return 63;
    }

    /* --- 复合赋值 += -= *= /= %= --- */
    /* (from 027_wrong_compound_assign) */
    {
        int x;
        x = 5; x += 3;
        if (x != 8) return 70;
        x = 5; x -= 3;
        if (x != 2) return 71;
        x = 5; x *= 3;
        if (x != 15) return 72;
        x = 15; x /= 4;
        if (x != 3) return 73;
        x = 15; x %= 4;
        if (x != 3) return 74;
    }

    /* --- 链式赋值 a = b = c --- */
    /* (from 028_assign_chain) */
    {
        int a, b, c;
        a = b = c = 42;
        if (a != 42) return 80;
        if (b != 42) return 81;
        if (c != 42) return 82;
    }

    return 0;
}
