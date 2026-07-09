# tcc — ToyCCompiler

从 [Tinylibc](https://github.com/WHU-SC7/Tinylibc) 项目提取、独立发展的 C 编译器。
约一万行 C，零 libc 依赖，已通过完整自举验证。

## 项目结构

```
├── app/                # 编译器源码
│   ├── tcc.c           # 主入口：编译 C → ELF .o
│   ├── tcc_rt.c        # 独立运行时（syscall 包装、malloc、printf）
│   ├── tcc_rt_start.S  # 启动汇编 __tlibc_start → main → exit
│   ├── lex.c           # 词法分析
│   ├── parse.c         # 递归下降解析
│   ├── preproc.c       # 预处理器（宏展开、#include、条件编译）
│   ├── cgen.c          # 代码生成（函数、流程控制）
│   ├── cgen_expr.c     # 表达式代码生成
│   ├── cgen_asm.c      # __asm__ 内联汇编
│   ├── elf_write.c     # ELF64 .o 文件写入
│   ├── elf_write.h     # ELF 写入器接口
│   ├── tcc.h           # 编译器核心类型定义
│   ├── tpp.c           # 独立预处理器
│   └── tas.c           # x86_64 汇编器
├── include/
│   ├── tcc_need.h      # 最小化类型/常量/系统调用宏/函数声明
│   └── elf.h           # ELF64 结构体定义
├── compiler-tests/     # 测试文件
│   ├── basic/          # 常规测试（tcc 编译 + tcc_rt 链接，28 个）
│   ├── selfhost/       # 自包含测试（tcc 独立编译，无 tcc_rt 依赖，34 个）
│   ├── source/         # 源文件独立测试（验证单个 .c 文件的逻辑，9 个）
│   └── pending/        # 待修复 bug 的复现用例
├── ld.script           # 链接脚本
├── Makefile            # 构建系统
└── bootstrap-selfhost.sh  # 自举自托管测试脚本
```

## 构建

```sh
make                         # 构建 tcc、tpp、tas → build/
make test                    # 常规测试（28 个）
make test 03                 # 指定编号测试
make test 03 07              # 多编号测试
make test-selfhost           # 自包含测试（34 个）
make test-source             # 源文件独立测试（gcc 编译，9 个）
make test-source-tcc         # 源文件独立测试（tcc 编译，9 个）
./bootstrap-selfhost.sh      # 自举：stage-1 → stage-2 → 测试
./bootstrap-to-10.sh         # 全链自举验证（stage-1 → stage-10）
make clean
```

## 测试状态（2026-07-08）

| 测试套件 | 通过/总数 | 说明 |
|----------|-----------|------|
| `make test` | 28/28 ✅ | tcc 编译 + ld 链接 tcc_rt 运行时 |
| `make test-selfhost` | **34/34 ✅** | tcc 独立编译，无 tcc_rt 依赖 |
| `make test-source`（gcc） | 9/9 ✅ | gcc 编译源文件独立测试 |
| `make test-source-tcc`（tcc） | 9/9 ✅ | tcc 编译源文件独立测试 |
| `bootstrap-selfhost.sh`（stage-2） | 31/31 ✅ | stage-2 tcc 编译 selfhost 测试 |
| `bootstrap-to-10.sh` | stage-3→10 字节级一致 ✅ | 全链收敛验证 |

## 设计原则

- **无 libc 依赖**：运行时通过 `syscall` 宏直接调用 Linux 内核
- **静态链接**：输出 ELF64 .o，`ld -nostdlib -static` 链接
- **简化优先**：源码写法向 tcc 自身能编译的方向靠拢
- **自举导向**：所有决策围绕"让 tcc 能编译自己"展开

## 已知限制

| 特性 | 状态 |
|------|------|
| 浮点运算 | 默认构建未启用（`make CFLAGS+=-DTCC_FLOAT` 开启） |
| VLA（变长数组） | 未实现 |
| 位域（bitfield） | 未实现 |
| 复合字面量 `(int[]){1,2}` | 未实现 |
| 指定初始化器 `.field=val` | 未实现 |
| `_Generic` | 未实现 |
| `long double` | 不支持 |
| `goto` 跨函数 | 未检查 |
| 宽字符/宽字符串 | 未实现 |

### 已修复的历史限制

以下问题曾在开发过程中存在，现已修复，对应测试持续回归验证：

| 问题 | 修复验证 |
|------|----------|
| `main()` 的 `return` 代码生成缺陷 | `compiler-tests/basic/*.c` 均正常使用 `main()` + `return` ✅ |
| 变参参数超过 5 个传参错误 | `selfhost/11_printf_manyargs.c` 验证 6–12 个参数 ✅ |
| `*(ptr + N)` 指针嵌套解引用按 1 字节加载 | `selfhost/20_ptr_arith.c` 验证 `int*`/`long*` 解引用 ✅ |
| `*(cast_type *)ptr = val` 存储宽度错误 | `selfhost/16_cast_deref_assign.c` 验证 ✅ |
| 结构体返回值数组元素赋值只复制 8 字节 | `selfhost/29_struct_return_expr.c` 等验证 ✅ |
| `Lexer` 结构体复制只复制 8 字节（自举阻断 bug） | 全链 stage-3→10 收敛验证 ✅ |

## 验证

```sh
make test             # 28/28 ✅
make test-selfhost    # 34/34 ✅
./bootstrap-to-10.sh  # stage-3 → stage-10 字节级完全一致 ✅
```

编译器输出在 stage-3 收敛，stage-3 到 stage-10 保持字节级一致——一次完整可验证的自举。
