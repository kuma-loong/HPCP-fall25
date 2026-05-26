# 实验6:OpenMP并行编程

## 1. 实验目的

- 掌握OpenMP编程模型和应用场景，使用OpenMP并行化DGEMM

- 理解OpenMP和pthreads编程方法的区别

## 2. 实验内容

- 运行示例代码，掌握OpenMP的基本使用。

- 在框架代码how-to-optimize-gemm中集成OpenMP实现DGEMM的版本，并记录相关数据。

## 3. 新增OpenMP实现

- 新增openmp.c，在naive gemm代码的基础上，增加openmp编译指导语句。注意循环变量的shared和private属性。

- 修改makefile：增加编译选项，使得能够处理openmp的编译指导语句；修改变量NEW的赋值；修改环境变量调整线程数目。

- -fopenmp选项编译和链接都需要

## 4. 实验报告及要求

提交代码和实验报告。

- 课程实验所有要求提交的代码，只提交自己写的或者有做改动的代码文件，其他未做改动的框架代码无需提交。

- 需要贴运行结果的，可以用截图，也可以直接从终端复制文本。

- 报告内容如下：

```text
课程信息：高性能计算应用实践、2025秋
实验名称：              上课地点：T2612
姓名：      学号：      班级： 

1、OpenMP实现
   - 代码，尽管提交了代码文件，这里仍要求直接贴一下MY_MMult函数的代码，包括makefile的修改。
   - 运行时该进程的CPU利用率截图
   - 调整线程数，按以下表格记录矩阵规模为1024时gflops数值。
   - 最少记录2、4、max个线程的数据，max是CPU物理核心的数量，若为4则无需再另外记录。
   |     实现            | 浮点性能（gflops）|
   | :---                |             ---: |  
   | OpenMP 2 threads    |                  |  
   | OpenMP 4 threads    |                  |  
   | OpenMP max threads  |                  |

2、 gflops曲线图，并对数据做分析说明
    - 基于框架代码`how-to-optimize-gemm`的naive、pthread、openmp、openblas四种方式得到的数据绘制在同一个图。
    - 画图可以用python plotAllfile.py，会自动读取_data目录下所有.m数据文件（不包括output_new.m和output_old.m）绘图。
    - 矩阵规模至少包括4，16，64，256，512，1024，2048。
    - 如果"矩阵规模/CPU核心数"的结果不是整数，可以调整矩阵规模比如1000，2000。
    - openmp和pthread的线程数设置为CPU物理核心数。
    - 以下面表格的形式列出相对于naive的加速比。
   |     实现       | 加速比(n=4)| 加速比(n=1024) | 
   | :---          |       ---: |          ---: | 
   | Naive gemm    |            |               | 
   | Pthread       |            |               | 
   | OpenMP        |            |               |
   | Openblas      |            |               |

3、 回答以下问题：

- 问题（1）：多个c代码文件中定义了相同的MY_MMult函数，编译的时候为什么没有报重复定义的错？
            链接生成可执行文件选择哪个版本的MY_MMult函数是makefile中的哪些代码决定的？
- 问题（2）：文件_data/output_MMult0.m中的性能数据是怎么生成的？c代码中只是将数据输出到终端并没有写入文件。
            请指出makefile中生成.m文件的代码。
- 问题（3）：将makefile中的`NEW  := MMult0` 改为 `NEW  := test_MMult` 是否能正常编译，为什么？

```
