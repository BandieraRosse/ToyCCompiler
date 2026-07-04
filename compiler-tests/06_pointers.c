// EXPECT: 0
// 指针：赋值、取地址、解引用、运算、数组、字符串
int main(void) {
    int global_x = 42;
    int global_arr[4];

    /* --- 指针赋值与读取 --- */
    /* (from 021_pointer_assign) */
    {
        int x = 42;
        int *p, *q;
        p = &x;
        if (*p != 42) return 1;
        *p = 10;
        if (x != 10) return 2;
        q = p;
        if (*q != 10) return 3;
    }

    /* --- 取局部变量地址 --- */
    /* (from 034_addr_of_local) */
    {
        int x = 42;
        int *p = &x;
        if (*p != 42) return 10;
        *p = 100;
        if (x != 100) return 11;
    }

    /* --- 取全局变量地址 --- */
    /* (from 035_addr_of_global) */
    {
        int *p = &global_x;
        if (*p != 42) return 20;
        global_arr[0] = 10;
        global_arr[1] = 20;
        int *q = &global_arr[0];
        if (*q != 10) return 21;
        if (*(q + 1) != 20) return 22;
    }

    /* --- 指针减法 --- */
    /* (from 037_pointer_sub) */
    {
        int arr[5];
        int *p = &arr[0], *q = &arr[4];
        if (q - p != 4) return 30;
        if (p - q != -4) return 31;
    }

    /* --- 数组声明与初始化 --- */
    /* (from 029_array_decl, 030_array_index) */
    {
        int arr[5] = {10, 20, 30, 40, 50};
        if (arr[0] != 10) return 40;
        if (arr[1] != 20) return 41;
        if (arr[2] != 30) return 42;
        if (arr[3] != 40) return 43;
        if (arr[4] != 50) return 44;
    }

    /* --- 数组遍历赋值 --- */
    /* (from 031_array_assign) */
    {
        int arr[5], i;
        for (i = 0; i < 5; i = i + 1) arr[i] = i * 10;
        if (arr[0] != 0)  return 50;
        if (arr[1] != 10) return 51;
        if (arr[4] != 40) return 52;
    }

    /* --- 多维数组 --- */
    /* (from 032_multidim_array) */
    {
        int arr[3][4];
        int i, j, n = 0;
        for (i = 0; i < 3; i = i + 1)
            for (j = 0; j < 4; j = j + 1)
                arr[i][j] = n++;
        if (arr[0][0] != 0)  return 60;
        if (arr[0][3] != 3)  return 61;
        if (arr[2][3] != 11) return 62;
    }

    /* --- 字符串字面量 --- */
    /* (from 036_string_literal) */
    {
        char *s = "Hello";
        if (s[0] != 'H') return 70;
        if (s[4] != 'o') return 71;
        if (s[5] != '\0') return 72;
    }

    return 0;
}
