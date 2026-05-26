# 实验3:Linux环境下调试与矩阵乘法优化

## 1. 实验目的

- 进一步熟悉Linux环境使用

- 掌握Linux下C语言开发常用的调试方法

- 了解矩阵乘法优化基本原理

## 2. 实验内容

- 阅读框架`how-to-optimize-gemm`代码，修改矩阵规模使得最大能跑到1024，间距inc如有需要可以自行调整，默认从PFIRST开始每次翻倍。

- 阅读框架代码，新建openblas.c，将openblas的实现集成到框架代码，并相应修改makefile，运行并记录数据。

- 阅读gcc手册了解编译优化选项，修改makefile，分别选择O0，O1，O2，O3不同的优化选项，运行并记录数据。

- 运行test_python_dgemm.py，记录相关数据。

数据的记录要求请查看最后的 实验报告要求。

## 3. 框架代码介绍

`how-to-optimize-gemm`目录下的文件及功能如下：

```text
tree ./how-to-optimize-gemm                                      
./
├── defs.h               # 参数定义
├── makefile             # 编译用的makefile
├── MMult0.c             # 方式0的实现，naive GEMM，同REF_MMul.c
├── MMult1.c             # 新增方式1的实现示例，与MMult0.c相同，仅作为示例，增加了fprintf输出 
├── plotAllfile.py       # Python代码读取目录下所有.m文件的数据画gflops曲线图
├── plotFlops.py         # Python代码只读取output_old.m和output_new.m文件数据画gflops曲线图
├── REF_MMult.c          # naive GEMM实现，用做结果校验
├── test_MMult.c         # 程序入口，分配存储空间、初始化矩阵、矩阵乘，并与参考实现（REF_MMult）进行结果对比，
├── test_python_dgemm.py # Python代码，测试Python用for循环和用numpy库跑GEMM的性能
└── util.c               # 一些工具函数

0 directories, 10 files

```

### 3.1 首次运行生成基线数据

makefile文件中第1-2行OLD 和NEW两个变量指定了对比的GEMM实现版本，版本的名称需与C代码文件名相同。首次运行时，OLD 和 NEW 都赋值MMult0用于生成对比用的基线数据`_data/output_MMult0.m`。

```text
OLD  := MMult0         # 旧版本实现，对应MMult0.c文件。
NEW  := MMult0         # 新版本实现，对应MMult0.c文件。

```

### 3.2 数据m文件说明

- 数据m文件在_data目录下，其中output_new.m、output_old.m是中间文件不用管。

- date是运行时间，可确认是否是新跑的数据，每次重新跑会覆盖之前的数据。

- version是代码版本，是makefile变量NEW定义的值。

- MY_MMult性能数据，每行3个值，第一个值是矩阵大小，第二个值是gflops值，第三个值是误差大小，需要留意误差的值，误差过大并不会直接终止，仍会正常执行，需要确保优化的版本功能是正确的。

开发过程中对于小矩阵的结果校验，可以用以下工具：

- [矩阵乘法在线计算](http://www.yunsuan.info/matrixcomputations/solvematrixmultiplication.html)，注意只计算A*B，没有加C

- [https://matrixcalc.org/](https://matrixcalc.org/)

- 使用python验证

### 3.3 新增MMult1.c实现说明

MMult1.c作为新增实现的例子，实现逻辑完全同MMult0.c，MMult1.c同样实现了MY_MMult函数，相比MMult0.c只是加了一行fprintf，将文件名输出到标准错误输出（屏幕）。

- 新增优化实现时，新建任意c文件xx.c比如MMult1.c，命名应能体现优化方法，文件名大小写不限，文件中实现MY_MMult函数，再将makefile中NEW变量替换为c文件名xx。

- 再将makefile中的NEW变量修改为新增的优化实现，再执行make run就会执行新增实现中的MY_MMult函数。

将makefile修改为如下，再运行make run，会生成_data/output_MMult1.m的文件，即MMult1.c中的实现的运行数据。

```text
OLD  := MMult0
NEW  := MMult1

```

### 3.4 新增openblas的实现

创建openblas.c文件，同样定义MY_MMult函数，MY_MMult函数内再调用cblas_dgemm接口实现矩阵乘。

makefile中将变量NEW修改为`NEW  := openblas`，运行`make run`，注意看输出提示，会有error，表明编译失败，没有得到最终的可执行文件。error信息提示链接的时候找不到cblas_dgemm的实现，
回顾下直接用gcc编译openblas的测试代码，需要加一个编译选项，请自行修改makefile的LDFLAGS变量增加对openblas的支持。

Note

如果新增实现中有`#include "defs.h"`，编译时会报错，则将该include语句在其他标准库头文件后边，尤其是`#include <cblas.h>`后边，因为defs.h定义的M，N，K与cblas.h中函数声明中的M，N，K有冲突。

### 3.5 绘制性能gflops曲线图

- 先执行`pip install matplotlib numpy` 安装matplotplib和numpy包。
  如果执行出错提示pip不存在则尝试`pip3 install matplotlib numpy`。执行python的时候同样，如果提示python不在则用python3替代。

- 再执行命令`python plotFlops.py`绘制出gflops性能曲线图，会弹出一个图形窗口并显示一张图片，plotFlops.py通过读取_data/output_old.m和output_new.m文件的数据进行绘制。
  该脚本对m文件中的数据格式有严格的要求，如果增加其他格式的数据，会导致读取错误。

[如果执行出现下面的错误](https://stackoverflow.com/questions/52346254/importerror-failed-to-import-any-qt-binding-python-tensorflow)：

```text
Traceback (most recent call last):
  File "<user_script>", line X, in <module>
    fig = figure(**fig_kw)
  ...
  File "/home/txx/.local/lib/python3.10/site-packages/matplotlib/backends/qt_compat.py", line 130, in <module>
    raise ImportError(
ImportError: Failed to import any of the following Qt binding modules: PyQt5, PySide2

```

先尝试执行`pip install PyQt5 PySide2`安装缺失的包，`PyQt5 PySide2`后面的5和2是版本号，要根据提示信息自行修改，再运行`python plotFlops.py`。如果还失败，使用以下命令重装。参考：

```text
pip uninstall matplotlib
python -m pip install --upgrade pip
pip install matplotlib

```

横坐标的最大值即峰值性能需要手动配置，根据所用PC调整对应参数，跟以下三个参数有关：

- 每个时钟周期执行的浮点操作数有关：可以用默认4

- 处理器的核数：lscpu 等工具查看

- 频率： lscpu 等工具查看

计算公式如下：

```text
max_gflops = nflops_per_cycle * nprocessors * GHz_of_processor

```

### 3.6 WSL运行GUI程序

在 WSL 启动的 Ubuntu 中运行`python plotFlops.py`会弹出一个图形窗口并显示一张图片。如果没有显示图片且命令行有错误提示，则表明当前的WSL不支持运行Linux GUI应用程序，需进行如下配置。

以管理员身份启动PowerShell，分别执行下面两条命令，即先更新wsl，再关闭WSL。

```text
wsl --update
wsl --shutdown

```

再启动WSL的Ubuntu，切到框架代码的目录，再执行`python plotFlops.py`。

若Windows版本较低，更新后还显示失败可尝试手动安装离线版，点击[wsl_graphics_support](https://github.com/microsoft/wslg/releases?page=3)下载wsl_graphics_support_x64.msi并安装。

![alt text](../../assets/lab3-image.png)

若最终仍不能显示图片，可以把代码中的`# fig.savefig("test.png")`注释去掉将图片保存到本地进行查看。

## 4. 实验报告要求

提交代码和实验报告。

- 课程实验所有要求提交的代码，只提交自己写的或者有做改动的代码文件，其他未做改动的框架代码无需提交。

- 需要贴运行结果的，可以用截图，也可以直接从终端复制文本。

- 报告内容如下：

```text
课程信息：高性能计算应用实践、2025秋
实验名称：              上课地点：T2612
姓名：      学号：      班级： 

1、实现说明
  - openblas实现的代码，makefile要如何改动。尽管提交了代码文件，这里仍要求直接贴一下代码。
  - 增加O1优化选项 makefile要如何改动

2、记录矩阵规模为1024时各个版本的数据，相对加速比都是相对版本1 python的实现。

| 版本  |     实现            | 浮点性能（gflops）| 相对加速比 |
| :--- | :---                |             ---: |  ---:     |  
| 1    | Python naive        |                  |           |  
| 2    | C naive             |                  |           |  
| 3    | C naive (O1)        |                  |           |
| 4    | C naive (O2)        |                  |           |  
| 5    | C naive (O3)        |                  |           |  
| 6    | OpenBlas            |                  |           |  
| 7    | Python numpy        |                  |           |

3、碰到的问题及解决办法。

```
