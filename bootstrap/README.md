# 自举种子（Bootstrap Seeds）

本目录包含 tcc 项目的自举种子二进制：

- `tcc` — C 编译器（206 KB，git 追踪）
- `tas` — x86_64 汇编器（42 KB，git 追踪）

## 用途

`bootstrap/tcc` 和 `bootstrap/tas` 是 Makefile 的默认工具链。
`make` 即用它们编译项目源码，产出 `build/tcc` 和 `build/tas`。

## 更新种子

```sh
make && make update-bootstrap
```

这会用当前源码构建 tcc+tas，然后覆盖 `bootstrap/` 中的种子。
通常在源码重大变更后执行。

## 自举验证

```sh
./bootstrap-selfhost.sh   # seed → stage-2 → 全部 selfhost 测试
./bootstrap-to-10.sh      # stage-3→10 字节级收敛验证
```

## 历史

种子最初由宿主机 C 编译器编译生成。自 stage-2 起，
tcc 自举版本与种子语义等价（字节级收敛）。项目已完全自举。
