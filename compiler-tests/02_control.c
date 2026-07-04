// EXPECT: 0
// 流程控制：if/else、while、for、do-while、switch/case、goto
int main(void) {
    /* --- if / else --- */
    /* (from 005_if_else) */
    {
        int r;
        if (1) r = 10; else r = 20;
        if (r != 10) return 1;
        if (0) r = 30; else r = 40;
        if (r != 40) return 2;
        /* 嵌套 if-else */
        int x = 1, y = 0;
        if (x) if (y) return 3; else r = 50;
        if (r != 50) return 4;
    }

    /* --- while --- */
    /* (from 006_while) */
    {
        int i = 0, sum = 0;
        while (i < 10) { sum = sum + i; i = i + 1; }
        if (sum != 45) return 10;
    }

    /* --- for --- */
    /* (from 007_for) */
    {
        int sum = 0, i;
        for (i = 0; i < 10; i = i + 1) sum = sum + i;
        if (sum != 45) return 20;
        /* for 中 continue */
        sum = 0;
        for (i = 0; i < 10; i = i + 1) {
            if (i == 5) continue;
            sum = sum + i;
        }
        if (sum != 40) return 21;
    }

    /* --- do-while --- */
    /* (from 008_do_while) */
    {
        int i = 0, sum = 0;
        do { sum = sum + i; i = i + 1; } while (i < 10);
        if (sum != 45) return 30;
    }

    /* --- break / continue --- */
    {
        int sum = 0, i;
        for (i = 0; i < 10; i = i + 1) {
            if (i == 7) break;
            sum = sum + i;
        }
        if (sum != 21) return 40;  /* 0+1+2+3+4+5+6 = 21 */
        /* continue */
        sum = 0;
        for (i = 0; i < 5; i = i + 1) {
            if (i == 2) continue;
            sum = sum + i;
        }
        if (sum != 8) return 41;  /* 0+1+3+4 = 8 */
    }

    /* --- switch / case / default --- */
    /* (from 052_switch, 053_switch_default) */
    {
        int r;
        switch (2) {
            case 1: r = 10; break;
            case 2: r = 20; break;
            default: r = 99;
        }
        if (r != 20) return 50;
        /* default */
        switch (99) {
            case 1: r = 10; break;
            default: r = 42;
        }
        if (r != 42) return 51;
    }

    /* --- goto --- */
    /* (from 054_goto) */
    {
        int x = 0;
        if (x != 0) goto fail;
        x = 10;
        goto ok;
    fail:
        return 60;
    ok:;
        if (x != 10) return 61;
    }

    return 0;
}
