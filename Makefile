#
# Makefile — tcc (Tinylibc C Compiler) 独立构建系统
#
# 从 app/ 编译 tcc/tpp/tas，输出到 build/
#
# 目标:
#   all       构建全部三个工具
#   clean     删除构建产物
#   test      运行编译器测试套件（指定编号）
#   test-all  运行全部测试（basic + selfhost + source）
#   test-all  运行全部测试
#
# 变量（可覆盖）:
#   CC      宿主 C 编译器              (默认: gcc)
#   LD      宿主链接器                 (默认: ld)
#   CFLAGS  额外 C 编译标志
#   LDFLAGS 额外链接标志
#   BUILD   构建输出目录               (默认: build)
#

# ─── 工具链 ────────────────────────────────────────────────────

CC      ?= gcc
LD      ?= ld
CFLAGS  ?= -nostdlib -ffreestanding -Wall -Wextra
LDFLAGS ?= -nostdlib -static -e __tlibc_start

# ─── 路径 ───────────────────────────────────────────────────────

BUILD   := build
SRC     := app
INC     := include

CFLAGS  += -I $(INC) -I $(SRC)

# tcc 最小化头文件（修改后触发重新编译所有目标文件）
TCC_NEED := $(INC)/tcc_need.h
ELF_H    := $(INC)/elf.h
ELF_W_H  := $(SRC)/elf_write.h
HEADERS  := $(TCC_NEED) $(ELF_H) $(ELF_W_H)

# ─── 目标定义（按 tmakelist 分类） ────────────────────────────

# tcc: 完整 C 编译器
TCC_OBJS := $(BUILD)/tcc.o           \
            $(BUILD)/tcc_rt.o        \
            $(BUILD)/tcc_rt_start.o  \
            $(BUILD)/lex.o           \
            $(BUILD)/parse.o         \
            $(BUILD)/preproc.o       \
            $(BUILD)/cgen.o          \
            $(BUILD)/cgen_expr.o     \
            $(BUILD)/cgen_asm.o      \
            $(BUILD)/elf_write.o

# tpp: 独立预处理器
TPP_OBJS := $(BUILD)/tpp.o           \
            $(BUILD)/tcc_rt.o        \
            $(BUILD)/tcc_rt_start.o  \
            $(BUILD)/preproc.o

# tas: x86_64 汇编器
TAS_OBJS := $(BUILD)/tas.o           \
            $(BUILD)/tcc_rt.o        \
            $(BUILD)/tcc_rt_start.o  \
            $(BUILD)/elf_write.o

# ─── 目标文件（去重后供编译/清理用） ─────────────────────────

ALL_OBJS := $(sort $(TCC_OBJS) $(TPP_OBJS) $(TAS_OBJS))

# ─── 默认目标 ──────────────────────────────────────────────────

.PHONY: all clean test test-all test-source test-source-tcc

all: $(BUILD)/tcc $(BUILD)/tpp $(BUILD)/tas
	@printf "$(GREEN)✓ 构建完成$(RESET) 输出在 $(BUILD)/: "; \
	ls -lh $(BUILD)/tcc $(BUILD)/tpp $(BUILD)/tas | awk '{printf "%s ", $$9}'

# ─── 创建 build 目录 ──────────────────────────────────────────

$(BUILD):
	mkdir -p $(BUILD)

# ─── C 编译规则 ────────────────────────────────────────────────
#
# 所有 .c 文件通过 tcc.h → tcc_need.h / elf.h 获取类型声明。
# parse.c 没有 #include，需要用 -include tcc.h 强制包含。
# tcc_rt.c 自包含，直接依赖 tcc_need.h。

# 通用规则：依赖头文件变化时重新编译
$(BUILD)/%.o: $(SRC)/%.c $(HEADERS) | $(BUILD)
	@printf "  $(BLUE)CC$(RESET)  %-20s " "$<"
	$(CC) $(CFLAGS) -MD -c $< -o $@
	@printf "$(GREEN)ok$(RESET)\n"

# parse.c: 没有 #include 行，强制包含 tcc.h
$(BUILD)/parse.o: $(SRC)/parse.c $(HEADERS) | $(BUILD)
	@printf "  $(BLUE)CC$(RESET)  %-20s " "$<"
	$(CC) $(CFLAGS) -include tcc.h -MD -c $< -o $@
	@printf "$(GREEN)ok$(RESET)\n"

# tcc_rt.c: 自包含运行时，仅依赖 tcc_need.h（不包含 tcc.h）
$(BUILD)/tcc_rt.o: $(SRC)/tcc_rt.c $(TCC_NEED) | $(BUILD)
	@printf "  $(BLUE)CC$(RESET)  %-20s " "$<"
	$(CC) $(CFLAGS) -MD -c $< -o $@
	@printf "$(GREEN)ok$(RESET)\n"

# ─── 汇编编译规则 ──────────────────────────────────────────────

$(BUILD)/tcc_rt_start.o: $(SRC)/tcc_rt_start.S | $(BUILD)
	@printf "  $(BLUE)AS$(RESET)  %-20s " "$<"
	$(CC) -x assembler-with-cpp -c $< -o $@
	@printf "$(GREEN)ok$(RESET)\n"

# ─── 链接规则 ──────────────────────────────────────────────────

$(BUILD)/tcc: $(TCC_OBJS)
	@printf "$(BLUE)  LD$(RESET) tcc ... "
	$(LD) $(LDFLAGS) $^ -o $@
	@printf "$(GREEN)ok$(RESET) ($(shell stat -c%s $@) bytes)\n"

$(BUILD)/tpp: $(TPP_OBJS)
	@printf "$(BLUE)  LD$(RESET) tpp ... "
	$(LD) $(LDFLAGS) $^ -o $@
	@printf "$(GREEN)ok$(RESET)\n"

$(BUILD)/tas: $(TAS_OBJS)
	@printf "$(BLUE)  LD$(RESET) tas ... "
	$(LD) $(LDFLAGS) $^ -o $@
	@printf "$(GREEN)ok$(RESET)\n"

# ─── 清理 ──────────────────────────────────────────────────────

clean:
	@printf "$(BLUE)  CLEAN$(RESET) removing objects and binaries ... "
	rm -f $(ALL_OBJS) $(ALL_OBJS:.o=.d)
	rm -f $(BUILD)/tcc $(BUILD)/tpp $(BUILD)/tas
	@printf "$(GREEN)done$(RESET)\n"
	rmdir $(BUILD) 2>/dev/null; true
	@printf "$(BLUE)  CLEAN$(RESET) removing logs ... "
	rm -rf tmp
	@printf "$(GREEN)done$(RESET)\n"

# ─── 全部测试 ──────────────────────────────────────────────────

test-all: test test-selfhost test-source test-source-tcc
	@printf "$(GREEN)✓ 全部测试通过$(RESET)\n"


# ─── 测试 ──────────────────────────────────────────────────────
#
# 用法：
#   make test           # 运行全部
#   make test 01 05     # 只跑指定编号
#
# 每个 compiler-tests/basic/*.c 首行的 // EXPECT: N 指定预期退出码。
# 流程：tcc 编译 → ld 链接 → 运行并检查退出码。

RED := \033[31m
GREEN := \033[32m
YELLOW := \033[33m
BLUE := \033[34m
RESET := \033[0m

TESTDIR := compiler-tests/basic
LDTESTFLAGS := -nostdlib -static -T ld.script

test: $(BUILD)/tcc $(BUILD)/tcc_rt.o $(BUILD)/tcc_rt_start.o
	@ok=0; fail=0; total=0; mkdir -p tmp; \
	ids=$(filter-out $@ test-all test-selfhost test-source test-source-tcc,$(MAKECMDGOALS)); \
	if [ -z "$$ids" ]; then files="$(TESTDIR)/*.c"; \
	else files=; for n in $$ids; do files="$$files $(TESTDIR)/$${n}_*.c"; done; fi; \
	printf "$(BLUE)══════ tcc 常规测试 ══════$(RESET)\n"; \
	if [ -n "$$ids" ]; then printf "  指定编号: $$ids\n"; fi; \
	printf "\n"; \
	for f in $$files; do \
		[ -f "$$f" ] || continue; \
		total=$$((total+1)); \
		name=$$(basename "$$f" .c); \
		expect=$$(sed -n 's/.*EXPECT: *\([0-9]*\).*/\1/p' "$$f" | head -1); \
		[ -z "$$expect" ] && expect=0; \
		printf "  tcc → $(BLUE)%-26s$(RESET) " "$$name"; \
		$(BUILD)/tcc "$$f" -o /tmp/$$name.o 2>/dev/null || { printf "$(RED)COMPILE FAIL$(RESET)\n"; fail=$$((fail+1)); continue; }; \
		$(LD) $(LDTESTFLAGS) /tmp/$$name.o $(BUILD)/tcc_rt.o $(BUILD)/tcc_rt_start.o -o /tmp/$$name 2>/dev/null || { printf "$(RED)LINK FAIL$(RESET)\n"; fail=$$((fail+1)); continue; }; \
		/tmp/$$name >tmp/$$name.log 2>&1; got=$$?; \
		if [ "$$got" = "$$expect" ]; then printf "$(GREEN)✓$(RESET) (%d)\n" "$$got"; ok=$$((ok+1)); \
		else printf "$(RED)✗$(RESET) (want %d got %d) — tmp/$$name.log\n" "$$expect" "$$got"; fail=$$((fail+1)); fi; \
	done; \
	printf "\n$(BLUE)══════$(RESET) $(GREEN)%d passed$(RESET), $(RED)%d failed$(RESET), %d total $(BLUE)══════$(RESET)\n" "$$ok" "$$fail" "$$total"; \
	[ "$$fail" -eq 0 ]

# ─── 依赖文件包含（-MD 自动生成的 .d 实现增量头文件跟踪） ──

-include $(ALL_OBJS:.o=.d)

# ─── 允许 make test 01 05 等带参数的目标 ─────────────────────

%:
	@:

# ─── 源文件独立测试（source）：验证单个源文件逻辑 ──
#
# 默认用 gcc 编译，也可用项目自身的 build/tcc：
#   make test-source                  # gcc 编译
#   make test-source SCTEST_USE_TCC=1 # build/tcc + ld 编译

SOURCETESTDIR := compiler-tests/source

SCTEST_CC       ?= gcc
SCTEST_CFLAGS   ?= -nostdlib -ffreestanding -Wall -Wextra -O0

# 使用 build/tcc 时的链接 flags（测试文件自包含，无需 tcc_rt.o）
SCTEST_LDFLAGS  := -nostdlib -static -T ld.script

# 根据 SCTEST_USE_TCC 选择依赖
ifeq ($(SCTEST_USE_TCC),)
test-source: $(SOURCETESTDIR)/lex.c
else
test-source: $(BUILD)/tcc
endif
	@ok=0; fail=0; total=0; mkdir -p tmp; \
	printf "$(BLUE)══════ source 测试（$(if $(SCTEST_USE_TCC),tcc,gcc) 编译）══════$(RESET)\n\n"; \
	for f in $(SOURCETESTDIR)/*.c; do \
		name=$$(basename "$$f" .c); \
		total=$$((total+1)); \
		expect=$$(sed -n 's/.*EXPECT: *\([0-9]*\).*/\1/p' "$$f" | head -1); \
		[ -z "$$expect" ] && expect=0; \
		printf "  $(BLUE)%-25s$(RESET) " "$$name"; \
		if [ -n "$(SCTEST_USE_TCC)" ]; then \
			$(BUILD)/tcc "$$f" -o /tmp/$$name.o 2>tmp/test_source_$$name-compile.log; \
			if [ $$? -ne 0 ]; then \
				printf "$(RED)COMPILE FAIL$(RESET)\n"; fail=$$((fail+1)); continue; \
			fi; \
			$(LD) $(SCTEST_LDFLAGS) /tmp/$$name.o -o /tmp/test_$$name 2>tmp/test_source_$$name-link.log; \
			if [ $$? -ne 0 ]; then \
				printf "$(RED)LINK FAIL$(RESET)\n"; fail=$$((fail+1)); continue; \
			fi; \
		else \
			$(SCTEST_CC) $(SCTEST_CFLAGS) -Wl,-e,__tlibc_start "$$f" -o /tmp/test_$$name 2>tmp/test_source_$$name-compile.log; \
			if [ $$? -ne 0 ]; then \
				printf "$(RED)COMPILE FAIL$(RESET)\n"; fail=$$((fail+1)); continue; \
			fi; \
		fi; \
		/tmp/test_$$name > tmp/test_source_$$name.log 2>&1; got=$$?; \
		if [ "$$got" = "$$expect" ]; then \
			printf "$(GREEN)✓$(RESET) (%d)\n" "$$got"; ok=$$((ok+1)); \
		else \
			printf "$(RED)✗$(RESET) (want %d got %d) — tmp/test_source_$$name.log\n" "$$expect" "$$got"; fail=$$((fail+1)); \
		fi; \
	done; \
	printf "\n$(BLUE)══════$(RESET) $(GREEN)%d passed$(RESET), $(RED)%d failed$(RESET), %d total $(BLUE)══════$(RESET)\n" "$$ok" "$$fail" "$$total"; \
	[ "$$fail" -eq 0 ]

# 用 tcc 编译源文件独立测试（免 SCTEST_USE_TCC=1 参数）
.PHONY: test-source-tcc
test-source-tcc: $(BUILD)/tcc
	$(MAKE) test-source SCTEST_USE_TCC=1


# ─── 自包含测试（selfhost）：tcc 独立编译，无 tcc_rt 依赖 ──

SELFTESTDIR := compiler-tests/selfhost

test-selfhost: $(BUILD)/tcc
	@ok=0; fail=0; total=0; mkdir -p tmp; 	printf "$(BLUE)══════ tcc 自包含测试（无 tcc_rt 依赖）══════$(RESET)\n\n"; 	for f in $(SELFTESTDIR)/*.c; do \
		[ -f "$$f" ] || continue; \
		total=$$((total+1)); \
		name=$$(basename "$$f" .c); \
		expect=$$(sed -n 's/.*EXPECT: *\([0-9]*\).*/\1/p' "$$f" | head -1); \
		[ -z "$$expect" ] && expect=0; \
		printf "  tcc → $(BLUE)%-26s$(RESET) " "$$name"; \
		$(BUILD)/tcc "$$f" -o /tmp/$$name.o 2>/dev/null || { printf "$(RED)COMPILE FAIL$(RESET)\n"; fail=$$((fail+1)); continue; }; \
		$(LD) $(LDTESTFLAGS) /tmp/$$name.o -o /tmp/$$name 2>/dev/null || { printf "$(RED)LINK FAIL$(RESET)\n"; fail=$$((fail+1)); continue; }; \
		/tmp/$$name >tmp/$$name.log 2>&1; got=$$?; \
		if [ "$$got" = "$$expect" ]; then printf "$(GREEN)✓$(RESET) (%d)\n" "$$got"; ok=$$((ok+1)); \
		else printf "$(RED)✗$(RESET) (want %d got %d) — tmp/$$name.log\n" "$$expect" "$$got"; fail=$$((fail+1)); fi; \
	done; \
	printf "\n$(BLUE)══════$(RESET) $(GREEN)%d passed$(RESET), $(RED)%d failed$(RESET), %d total $(BLUE)══════$(RESET)\n" "$$ok" "$$fail" "$$total"; \
	[ "$$fail" -eq 0 ]
