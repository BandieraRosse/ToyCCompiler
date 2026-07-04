// EXPECT: 0
// member_access.c — 测试结构体成员访问和指针成员访问
// 自举关键：大量使用 s.member 和 p->member

typedef struct { int x; int y; } Point;
typedef struct { Point start; Point end; } Line;

int main(void) {
    Point p;
    p.x = 10;
    p.y = 20;

    /* 1. . 成员访问 */
    if (p.x != 10) return 1;
    if (p.y != 20) return 2;

    /* 2. -> 成员访问 */
    Point *pp = &p;
    if (pp->x != 10) return 10;
    if (pp->y != 20) return 11;

    /* 3. 赋值后重读 */
    pp->x = 30;
    if (p.x != 30) return 20;

    /* 4. 嵌套结构体 */
    Line line;
    line.start.x = 1;
    line.start.y = 2;
    line.end.x = 3;
    line.end.y = 4;

    if (line.start.x != 1) return 30;
    if (line.end.y != 4) return 31;

    /* 5. 嵌套指针 */
    Line *lp = &line;
    if (lp->start.x != 1) return 40;
    if (lp->end.y != 4) return 41;

    /* 6. struct 作为函数参数 */
    /* 使用指针传递 */
    {
        Point *pp2 = &p;
        if (pp2->x + pp2->y != 50) return 50;
    }

    /* 7. 链式成员 */
    lp->start.x = 100;
    if (line.start.x != 100) return 60;

    return 0;
}
