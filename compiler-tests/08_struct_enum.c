// EXPECT: 0
// 结构体、联合、枚举、typedef
int main(void) {
    /* --- struct 声明与成员访问 --- */
    /* (from 046_struct_decl) */
    {
        struct Point { int x; int y; };
        struct Point p;
        p.x = 10;
        p.y = 20;
        if (p.x != 10) return 1;
        if (p.y != 20) return 2;
        /* 匿名 struct 成员 */
        struct { int a; int b; int c; } s;
        s.a = 1; s.b = 2; s.c = 3;
        if (s.a != 1) return 3;
        if (s.c != 3) return 4;
    }

    /* --- struct 赋值 --- */
    /* (from 047_struct_assign) */
    {
        struct Data { int a; int b; int c; };
        struct Data d1 = {10, 20, 30};
        struct Data d2;
        d2 = d1;
        if (d2.a != 10) return 10;
        if (d2.b != 20) return 11;
        if (d2.c != 30) return 12;
        d1.a = 99;
        if (d2.a != 10) return 13;  /* d2 独立副本 */
    }

    /* --- struct 指针与 -> --- */
    /* (from 048_struct_ptr) */
    {
        struct Point { int x; int y; };
        struct Point p = {10, 20};
        struct Point *ptr = &p;
        if (ptr->x != 10) return 20;
        if (ptr->y != 20) return 21;
        ptr->x = 30;
        if (p.x != 30) return 22;
    }

    /* --- struct 作为函数参数 --- */
    /* (from 049_struct_func_arg) */
    {
        struct Point { int x; int y; };
        int sum_coords(struct Point p) { return p.x + p.y; }
        struct Point p = {10, 20};
        if (sum_coords(p) != 30) return 30;
    }

    /* --- struct 作为函数返回值 --- */
    /* (from 050_struct_return) */
    {
        struct Point { int x; int y; };
        struct Point make(int a, int b) {
            struct Point p = {a, b};
            return p;
        }
        struct Point p = make(5, 7);
        if (p.x != 5) return 40;
        if (p.y != 7) return 41;
    }

    /* --- union --- */
    /* (from 051_union) */
    {
        union Data { int i; char c; };
        union Data u;
        u.i = 0x12345678;
        if (u.i != 0x12345678) return 50;
        u.c = 0xAA;
        /* char 只改一个字节，int 会变化 */
        if (u.i == 0x12345678) return 51;
    }

    /* --- enum --- */
    /* (from 055_enum) */
    {
        enum Color { RED, GREEN, BLUE };
        enum Color c = GREEN;
        if (RED   != 0)  return 60;
        if (GREEN != 1)  return 61;
        if (BLUE  != 2)  return 62;
        if (c != 1)      return 63;
        enum { A = 10, B, C = 20 };
        if (A != 10) return 64;
        if (B != 11) return 65;
        if (C != 20) return 66;
    }

    /* --- typedef --- */
    /* (from 056_typedef) */
    {
        typedef int integer;
        typedef unsigned char byte;
        integer x = 42;
        if (x != 42) return 70;
        if (sizeof(byte) != 1) return 71;
        typedef struct { int x; int y; } Point;
        Point p = {10, 20};
        if (p.x != 10) return 72;
    }

    return 0;
}
