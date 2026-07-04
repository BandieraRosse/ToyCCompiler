// EXPECT: 0
// 全局变量、静态函数、外部声明
int g_val;
int g_arr[3];
int g_a = 10, g_b = 20, g_c = 30;

static int internal_add(int a, int b) { return a + b; }
static int internal_val = 42;

extern int external_var;
void set_external(int x) { external_var = x; }

int main(void) {
    /* --- 全局变量读写 --- */
    /* (from 057_global_var) */
    {
        g_val = 42;
        if (g_val != 42) return 1;
        g_arr[0] = 10;
        g_arr[1] = 20;
        g_arr[2] = 30;
        if (g_arr[0] != 10) return 2;
        if (g_arr[2] != 30) return 3;
    }

    /* --- 全局变量初始化 --- */
    /* (from 058_global_init) */
    {
        if (g_a != 10) return 10;
        if (g_b != 20) return 11;
        if (g_c != 30) return 12;
    }

    /* --- static 函数与变量 --- */
    /* (from 059_static_func) */
    {
        if (internal_add(10, 20) != 30) return 20;
        if (internal_val != 42) return 21;
    }

    /* --- extern 声明 --- */
    /* (from 060_extern_decl) */
    {
        /* external_var 未在别处定义，应能被引用（链接阶段需定义） */
        set_external(99);
    }

    return 0;
}
