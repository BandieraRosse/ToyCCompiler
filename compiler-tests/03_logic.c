// EXPECT: 0
// 逻辑运算、比较、位运算、三目、逗号
int main(void) {
    /* --- 逻辑运算 && || ! --- */
    /* (from 009_logical) */
    {
        if (!(1 && 1)) return 1;
        if (1 && 0)    return 2;
        if (0 && 1)    return 3;
        if (!(0 || 1)) return 4;
        if (0 || 0)    return 5;
        if (!1)        return 6;
        if (!0 != 1)   return 7;
        /* 短路 */
        int x = 5;
        if (0 && (x = 10)) ;
        if (x != 5) return 8;
        x = 5;
        if (1 || (x = 10)) ;
        if (x != 5) return 9;
    }

    /* --- 比较 == != < > <= >= --- */
    /* (from 010_comparison) */
    {
        int x = 5, y = 10;
        if (!(5 == 5))     return 10;
        if (5 == 6)        return 11;
        if (!(5 != 6))     return 12;
        if (5 != 5)        return 13;
        if (!(5 < 6))      return 14;
        if (6 < 5)         return 15;
        if (!(6 > 5))      return 16;
        if (5 > 6)         return 17;
        if (!(5 <= 5))     return 18;
        if (!(5 <= 6))     return 19;
        if (6 <= 5)        return 20;
        if (!(5 >= 5))     return 21;
        if (!(6 >= 5))     return 22;
        if (5 >= 6)        return 23;
        if (x == y)        return 24;
        if (!(x != y))     return 25;
        if (!(x < y))      return 26;
        if (!(y > x))      return 27;
    }

    /* --- 位运算 & | ^ ~ << >> --- */
    /* (from 011_bitwise) */
    {
        int a = 0x0F, b = 0x33;
        if ((a & b) != 0x03) return 30;
        if ((a | b) != 0x3F) return 31;
        if ((a ^ b) != 0x3C) return 32;
        if ((~a & 0xFF) != 0xF0) return 33;
        if ((a << 2) != 0x3C) return 34;
        if ((0xF0 >> 2) != 0x3C) return 35;
    }

    /* --- 三目运算符 ? : --- */
    /* (from 012_ternary) */
    {
        int x;
        x = (1 ? 10 : 20);
        if (x != 10) return 40;
        x = (0 ? 10 : 20);
        if (x != 20) return 41;
        x = (1 ? 1 ? 30 : 40 : 50);
        if (x != 30) return 42;
    }

    /* --- 逗号表达式 --- */
    /* (from 013_comma) */
    {
        int x;
        x = (1, 2);
        if (x != 2) return 50;
        x = (1, 2, 3, 4, 5);
        if (x != 5) return 51;
    }

    return 0;
}
