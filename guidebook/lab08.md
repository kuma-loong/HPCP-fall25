# 实验8:HPL测试

## 1. 实验目的

- 掌握CPU峰值浮点计算性能的理论计算和测试

- 掌握HPL测试计算机系统浮点计算性能的原理、部署和参数优化

## 2. 实验内容

- CPU峰值浮点计算性能测试

- 系统实际浮点计算性能HPL测试

## 3. CPU峰值浮点计算性能测试

根据自己所用CPU的指令集，使用以下开源的测试工具进行峰值浮点性能测试。将代码clone或者下载到本地，按照说明编译、运行，记录输出结果。

- [https://github.com/pigirons/cpufp](https://github.com/pigirons/cpufp)：支持x86-64、arm64、riscv64、loongarch64等主流指令集的CPU浮点峰值性能测试工具

  - 通过`--thread_pool`指定CPU编号，比如--thread_pool=[0-3] 启动 4 个线程分别绑定到CPU 0-3

  - 输出结果会有多行对应到不同的向量指令集等，先筛选Core Computation 有f64的数据，然后再选大的值。

cpufp测试用例只有计算指令，无访存指令，故跑出来的值接近理论峰值性能。对于X86 CPU，支持以下指令集：

| Arch | ISA | Feature | Data Type | Description |
| --- | --- | --- | --- | --- |
| SIMD | SSE | Vector | fp32 | Before Sandy Bridge |
| SIMD | SSE2 | Vector | fp64 | Before Sandy Bridge |
| SIMD | AVX | Vector | fp32/fp64 | From Sandy Bridge |
| SIMD | FMA | Vector | fp32/fp64 | From Haswell/Zen |
| SIMD | AVX512f | Vector | fp32/fp64 | From Skylake X/Zen4 |
| SIMD | AVX512_VNNI | Vector | int8/int16 | From IceLake |
| SIMD | AVX_VNNI | Vector | int8/int16 | From Alder Lake |
| SIMD | AVX512_FP16 | Vector | fp16 | From Intel Sapphire Rapids |
| SIMD | AVX512_BF16 | Vector | bf16 | From AMD Zen4 |
| SIMD | AVX_VNNI_INT8 | Vector | int8 | From Intel Lion Cove |
| DSA | AMX_INT8 | Matrix | int8 | From Intel Sapphire Rapids |
| DSA | AMX_BF16 | Matrix | bf16 | From Intel Sapphire Rapids |
| DSA | AMX_FP16 | Matrix | fp16 | From Intel Granite Rapids |

cpufp会自动检测支持的指令集并运行对应的测试，输出结果会有多行，只有同时在lscpu flags中和以上表格中才会有对应的结果输出。比如以下输出

```text
--------------------------------------------------------------
| Instruction Set | Core Computation      | Peak Performance |
| AVX512F         | FMA(f32,f32,f32)      | 614.87 GFLOPS    |
| AVX512F         | FMA(f64,f64,f64)      | 372.94 GFLOPS    |
| FMA             | FMA(f32,f32,f32)      | 403.06 GFLOPS    |
| FMA             | FMA(f64,f64,f64)      | 199.38 GFLOPS    |
| AVX             | ADD(MUL(f32,f32),f32) | 196.56 GFLOPS    |
| AVX             | ADD(MUL(f64,f64),f64) | 101.4 GFLOPS     |
--------------------------------------------------------------

```

- AVX512F，用的是[x64/asm/AVX512F.S](https://github.com/pigirons/cpufp/blob/master/x64/asm/_AVX512F_.S)代码测试，vfmadd231pd融合乘加指令和512位的ZMM寄存器，测的是512位的FMA

- FMA，用的是[x64/asm/FMA.S](https://github.com/pigirons/cpufp/blob/master/x64/asm/_FMA_.S)代码测试，vfmadd231pd融合乘加指令和256位的XMM/YMM寄存器，测的是256位的FMA

- AVX，用的是[x64/asm/AVX.S](https://github.com/pigirons/cpufp/blob/master/x64/asm/_AVX_.S)代码测试，vmulpd 和 vaddpd 指令，即乘法和加法指令交替执行，只测256位的向量加法和乘法

注意

对于X86 CPU，如果输出有AVX512F，就选AVX512F的，如果没有，则选FMA的结果，通常FMA的结果是AVX的2倍左右，具体跟CPU的架构有关。

用lscpu或lstopo查看CPU信息，lscpu的输出重点关注以下几个与CPU数量有关的参数：

- CPU(s)：核心数=Socket 数 × 每个 Socket 的物理核数 × 每个核的线程数（超线程数）

- Thread(s) per core：每个物理核心能同时运行多少线程（如果支持超线程就是 2否则就是1）

- Core(s) per socket：每个处理器插槽（Socket）上的物理核心数

- Socket(s)： 物理 CPU 插槽数量（实际安装的 CPU 数量），笔记本、台式机只有一颗CPU，服务器可以装2颗、4颗、甚至8颗。

cpufp测试该用物理核心数还是逻辑核心数？不同的处理器尤其是不同指令集架构处的理器有区别，建议两种情况都测一下，取两者中较大的值。

现在流行的大小核架构、性能核/效能核架构，不同架构的核性能不一样，cpufp不会自动做负载均衡，可以手动测试，先指定大核的cpu id测大核的性能，再指定小核的cpu id测小核的性能，比起直接测试全部核的性能数据会更准确。

## 4. 系统实际浮点计算性能HPL测试

### 4.1 HPL安装

HPL依赖BLAS库和MPI框架，这两个依赖前面的实验已经有安装，openblas和MPICH，无需重复安装。如需要替换成其他开源版本，则相关路径做对应的替换。

执行以下命令：完成hpl压缩包的下载和解压，最终解压到~/hpl路径下。

```text
wget https://netlib.org/benchmark/hpl/hpl-2.3.tar.gz
gunzip hpl-2.3.tar.gz
tar xvf hpl-2.3.tar
rm hpl-2.3.tar
mv hpl-2.3 ~/hpl

```

生成Make.linux配置文件：

```text
cd ~/hpl/setup
sh make_generic
cp Make.UNKNOWN ../Make.linux
cd ../
vi Make.linux

```

上面最后一条命令用vi打开Make.linux文件，也可以用vscode打开。找到以下的行，将`=`后的内容修改为对应内容，`#`号后面的是注释。下面配置中涉及到的路径一定要ls查看下对应的文件和目录在不在。如果不在，检查是否安装了mpich和openblas，以及是否用 apt install安装。

```text
ARCH         = linux     # 架构名字，与文件名Make.linux的后缀相同

MPdir        =         # mpi的安装路径，如果MPinc和MPlib没引用可以为空，这里没有用上
MPinc        = -I/usr/include/x86_64-linux-gnu/mpich  # MPI头文件路径
MPlib        = /usr/lib/x86_64-linux-gnu/libmpich.so  # MPI库文件路径

LAdir        =   # BLAS库的安装路径
LAinc        = -I/usr/include/x86_64-linux-gnu/  # BLAS库的头文件
LAlib        = /usr/lib/x86_64-linux-gnu/libopenblas.so  # BLAS库的库文件

```

编辑Make.top文件，同样找到ARCH的定义改为linux。

```text
ARCH         = linux 

```

执行`make arch=linux` 编译，注意检查输出是否有error，如有error则编译失败，先检查以上配置是否有漏修改的，再根据提示信息查找原因，修改无误后先运行`make clean_arch_all`或 `make clean_arch_all arch=linux` 清除已有的编译文件，再 `make arch=linux` 重新编译。

编译成功会在`~/hpl`目录下生成`bin/linux/xhpl`可执行文件。

### 4.2 HPL编译运行

修改配置文件HPL.dat，相同的环境，参数不同，跑出来的结果不同，参数的设置跟系统的硬件配置有关，HPL的优化包括对HPL.dat参数进行优化。

```text
cd ~/hpl/bin/linux
vi HPL.dat

```

根据需求修改，比如只用单核cpu的跑，设置P=1 and Q=1，然后运行`./xhpl`启动测试，运行结束后会将结果直接输出到屏幕。

如果要启动多核多进程测试，需要用mpirun运行。比如设置P=2 and Q=2，则需要4个进程。用mpirun启动的时候-n指定的进程数要不小于P*Q的值。

```text
mpirun -n 4 ./xhpl

```

运行结果的开始和结束的输出如下。

![alt text](../../assets/lab8-image.png)

![alt text](../../assets/lab8-image-1.png)

运行的tests的数量跟配置有关，是N、NB、PQ取值可能性的乘积。取最大的值作为最终的测量值，跑出来的结果跟参数调优有关，具体参数设置可以参考：

- FAQ： [https://www.netlib.org/benchmark/hpl/faqs.html](https://www.netlib.org/benchmark/hpl/faqs.html)

- 自动调参工具：[https://www.advancedclustering.com/act_kb/tune-hpl-dat-file/](https://www.advancedclustering.com/act_kb/tune-hpl-dat-file/) ， 主要是节点数、CPU核数、内存、块大小。

主要参考：

- [HPL Software Download and Installation](https://www.netlib.org/benchmark/hpl/software.html)

- [How to compile HPL LINPACK on Ubuntu 22.04](https://www.mgaillard.fr/2022/08/27/benchmark-with-hpl.html)

### 4.3 可能碰到的问题

如果出现下面的问题，运行`make clean_arch_all` 或 `make clean_arch_all arch=linux`清除已有的编译结果尝试重新编译运行。

```text
$ ~/hpl/bin/linux >> ./xhpl
Abort(66671365) on node 0 (rank 0 in comm 0): Fatal error in internal_Comm_rank: Invalid communicator, error stack Internal_Comm_rank(74): MPI_Comm_rank(comm=0xe0d96260,rank=0x7fffd48837a4) failed
Internal_Comm_rank(41): Invalid communicator

```

如果出现下面的问题：先确认指定的-n指定的进程数是否大于PQ的乘积。如果没问题，则将PQ都改为1，直接./xhpl启动，会自动开启CPU核数对应的进程数，可以top查看cpu的利用率检查。

现象1：

```text
/usr/bin/mpirun -np 2 -machinefile
>>>>> machines /home/danield/xhpl
>>>>> HPL ERROR from process # 0, on line 419 of function HPL_pdinfo:
>>>>>>>> Need at least 2 processes for these tests <<<
>>>>>
>>>>> HPL ERROR from process # 0, on line 621 of function HPL_pdinfo:
>>>>>>>> Illegal input in file HPL.dat. Exiting ... <<<

```

现象2：

```text
ERROR from process # 0, on line 395 of function HPL_pdinfo:
Value of P less than 1

ERROR from process # 0, on line 621 of function HPL_dadmin:
Illegal input in file HPL.dat. Exiting ...

```

## 5. 实验报告及要求

提交代码和实验报告。

- 代码提交HPL.dat文件。

- 需要贴运行结果的，可以用截图，也可以直接从终端复制文本。

- 报告内容如下：

```text
课程信息：高性能计算应用实践、2025秋
实验名称：              上课地点：T2612
姓名：      学号：      班级： 

1. 系统硬件配置：内存大小，cpu型号、核数、频率、SIMD指令集版本，SIMD比如x86的avx2、avx512f，
2. CPU理论峰值浮点计算性能计算过程和结果，整个CPU的性能非单核
3. CPU峰值浮点计算性能测试结果以及`实测CPU峰值浮点计算性能/CPU理论峰值浮点计算性能`的比值
4. 软件环境：操作系统版本，gcc、MPI、数学库的版本
5. HPL参数调优过程和说明
6. 最优的测试结果以及`测试结果/CPU理论峰值浮点计算性能`的比值
7. 碰到的问题及解决方法
8. 在实验3表格的基础上，增加Multi threads、OpenMP的数据，并补充“达到峰值性能的百分比”。

| 版本  |     实现      | 浮点性能（gflops）| 相对加速比 |达到峰值性能的百分比 |   
| :--- | :---          |             ---: |  ---:     |        ---:       |
| 1    | Python naive  |                  |           |                   |
| 2    | C naive       |                  |           |                   |
| 3    | C naive (O1)  |                  |           |                   |
| 4    | C naive (O2)  |                  |           |                   |
| 5    | C naive (O3)  |                  |           |                   |
| 6    | Multi threads |                  |           |                   |
| 7    | OpenMP        |                  |           |                   |
| 8    | OpenBlas      |                  |           |                   |
| 9    | Python numpy  |                  |           |                   |

```
