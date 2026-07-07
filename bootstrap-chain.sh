#!/bin/bash
# bootstrap-chain.sh — 从 stage-1 到 stage-N 的完整自举链
# 用法: bash bootstrap-chain.sh [起始stage] [终止stage]
# 默认从stage-1(gcc)构建到stage-5

set -e

SRC="app"
STAGE_BASE="build/stages"
LDFLAGS="-nostdlib -static -e __tlibc_start"
LD="ld"

MIN_STAGE=${1:-1}
MAX_STAGE=${2:-5}

mkdir -p "$STAGE_BASE"

# stage-1: gcc 构建的 build/tcc（确保是最新）
if [ "$MIN_STAGE" -le 1 ]; then
    echo "=== [stage-1] gcc 构建 ==="
    # 确保 build/tcc 是最新的
    make -j$(nproc) 2>&1 | tail -1
    cp build/tcc "$STAGE_BASE/tcc-stage1"
    echo "  → $STAGE_BASE/tcc-stage1 ($(stat -c%s "$STAGE_BASE/tcc-stage1") bytes)"
fi

for stage in $(seq $((MIN_STAGE + 1)) $MAX_STAGE); do
    prev=$((stage - 1))
    prev_tcc="$STAGE_BASE/tcc-stage${prev}"
    stage_dir="$STAGE_BASE/stage${stage}"
    stage_tcc="$STAGE_BASE/tcc-stage${stage}"

    if [ ! -x "$prev_tcc" ]; then
        echo "ERROR: $prev_tcc not found or not executable"
        exit 1
    fi

    echo ""
    echo "=== [stage-${stage}] ${prev_tcc} → ${stage_tcc} ==="

    mkdir -p "$stage_dir"

    # 编译所有 C 源文件
    COMPILE_OK=0
    COMPILE_FAIL=0
    for cfile in $SRC/tcc.c $SRC/lex.c $SRC/parse.c $SRC/preproc.c \
                 $SRC/cgen.c $SRC/cgen_expr.c $SRC/cgen_asm.c \
                 $SRC/elf_write.c $SRC/tcc_rt.c; do
        bname=$(basename "$cfile" .c)
        ofile="$stage_dir/${bname}.o"
        if "$prev_tcc" "$cfile" -o "$ofile" 2>"${stage_dir}/compile_${bname}.log"; then
            COMPILE_OK=$((COMPILE_OK + 1))
        else
            echo "  COMPILE FAIL: ${bname}"
            COMPILE_FAIL=$((COMPILE_FAIL + 1))
        fi
    done

    # 汇编 tcc_rt_start.S（永远用 gcc）
    gcc -x assembler-with-cpp -c "$SRC/tcc_rt_start.S" \
        -o "$stage_dir/tcc_rt_start.o" 2>"${stage_dir}/compile_rt_start.log"

    if [ "$COMPILE_FAIL" -gt 0 ]; then
        echo "  ✗ $COMPILE_FAIL 个模块编译失败"
        exit 1
    fi

    # 链接
    $LD $LDFLAGS \
        "$stage_dir/tcc.o" "$stage_dir/lex.o" "$stage_dir/parse.o" \
        "$stage_dir/preproc.o" "$stage_dir/cgen.o" "$stage_dir/cgen_expr.o" \
        "$stage_dir/cgen_asm.o" "$stage_dir/elf_write.o" "$stage_dir/tcc_rt.o" \
        "$stage_dir/tcc_rt_start.o" \
        -o "$stage_tcc" 2>"${stage_dir}/link.log"

    echo "  → $stage_tcc ($(stat -c%s "$stage_tcc") bytes)"

    # 快速冒烟测试：编译并运行最简单的 selfhost 测试
    echo "  冒烟测试: 01_totally_selfcontained..."
    if "$stage_tcc" compiler-tests/selfhost/01_totally_selfcontained.c \
            -o "/tmp/stage${stage}_smoke.o" 2>/dev/null; then
        if $LD -nostdlib -static -T ld.script "/tmp/stage${stage}_smoke.o" \
                -o "/tmp/stage${stage}_smoke" 2>/dev/null; then
            "/tmp/stage${stage}_smoke" > /dev/null 2>&1
            if [ $? -eq 0 ]; then
                echo "  ✓ 冒烟测试通过"
            else
                echo "  ✗ 冒烟测试运行失败 (exit=$?)"
                exit 1
            fi
        else
            echo "  ✗ 冒烟测试链接失败"
            exit 1
        fi
    else
        echo "  ✗ 冒烟测试编译失败"
        exit 1
    fi
done

echo ""
echo "=== 自举链完成 ==="
for s in $(seq $MIN_STAGE $MAX_STAGE); do
    t="$STAGE_BASE/tcc-stage${s}"
    echo "  stage-${s}: $t ($(stat -c%s "$t") bytes)"
done
