# 25 秋季高性能计算实践实验课

本仓库整理了 2025 年秋季《高性能计算实践》课程的实验代码、课程指导书和相关资料。

## 课程介绍

课程围绕 Linux 环境下的性能分析、并行编程与矩阵乘法优化展开，覆盖 C 语言基础、调试、进程/线程、OpenMP、MPI 以及应用优化等内容。

## 实验说明

- `lab08` 为 HPL 测试，仓库中未单独整理。
- `lab09` 为基于华为鲲鹏处理器的性能测试与移植，内容与前序实验衔接紧密，未单独展开整理。
- `lab03` 到 `lab06`、`lab10` 的代码大多基于 `lab03/how-to-optimize-gemm` 框架，目录中会保留一些共用文件。

## 仓库结构

- `guidebook/`: 课程指导书与各实验说明。
- `lab01/`: Linux 操作系统与 shell 命令相关实验。
- `lab02/`: Linux 环境下 C 语言编程相关实验。
- `lab03/`: 调试与矩阵乘法优化相关实验。
- `lab04/`: Linux 环境多进程编程相关实验。
- `lab05/`: Linux 环境多线程编程相关实验。
- `lab06/`: OpenMP 并行编程相关实验。
- `lab07/`: MPI 相关实验。
- `lab03/how-to-optimize-gemm/`: 矩阵乘法优化主框架。

## 参考资料

- [课程指导书](guidebook/README.md)
- [实验 1](guidebook/lab01.md)
- [实验 2](guidebook/lab02.md)
- [实验 3](guidebook/lab03.md)
- [实验 4](guidebook/lab04.md)
- [实验 5](guidebook/lab05.md)
- [实验 6](guidebook/lab06.md)
- [实验 7](guidebook/lab07.md)
- [实验 8](guidebook/lab08.md)
- [实验 9](guidebook/lab09.md)
- [实验 10](guidebook/lab10.md)
