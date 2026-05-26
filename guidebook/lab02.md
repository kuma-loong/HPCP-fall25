# 实验2:Linux环境下C语言编程

## 1. 实验目的

- 熟悉Linux下可执行文件的生成过程

- 掌握gcc的使用

- 掌握Linux下C语言开发

## 2. 实验内容

![alt text](../../assets/lab2-image.png)

- 安装开源openblas库，编译执行test_cblas_dgemm.c。熟悉cblas_dgemm的接口参数，test_cblas_dgemm.c用的列主序的方式，请改为行主序后重新编译运行。

- time_dgemm.c中新增naive_dgemm实现，并在代码中新增与cblas_dgemm计算结果做对比，验证naive_dgemm实现正确。

$$
C = A \times B = 
\begin{pmatrix}
a_{11} & a_{12} \\
a_{21} & a_{22}
\end{pmatrix}
\times
\begin{pmatrix}
b_{11} & b_{12} \\
b_{21} & b_{22}
\end{pmatrix}
=
\begin{pmatrix}
a_{11}b_{11} + a_{12}b_{21} & a_{11}b_{12} + a_{12}b_{22} \\
a_{21}b_{11} + a_{22}b_{21} & a_{21}b_{12} + a_{22}b_{22}
\end{pmatrix}
$$

## 3. VS Code 安装配置

Visual Studio Code（VS Code）是由微软开发，同时支持 Windows、Linux 和 macOS 等操作系统且开放源代码的代码编辑器，用 TypeScript 编写，支持多种语言的开发。VS Code是一款应用非常广泛的IDE，强烈建议大家使用VS Code配合Linux环境进行开发。

### 3.1 安装

下载对应的安装包，下载完成后双击运行，默认的安装路径在C盘。

- VS Code安装包下载：[https://code.visualstudio.com/download](https://code.visualstudio.com/download)

- 安装说明文档：[https://code.visualstudio.com/docs/setup/windows](https://code.visualstudio.com/docs/setup/windows)

### 3.2 配置 WSL 远程开发

VS Code的远程开发功能非常好用，在Windows端启动VS Code，通过ssh连接到远端Linux服务器，提供在Linux本地开发的体验，VS Code直接编译远端服务器的文件，文件更新后在终端执行对应的编译、运行命令。

![alt text](../../assets/lab2-image-2.png)

WSL Ubuntu 在实验1中已安装好，只需要参照官方教程做以下配置：

- Install the WSL extension， VS Code 左边工具栏有个Extensions图标（快捷键是Ctrl+Shift+X），输入WSL即可搜索到对应的插件，点击安装。

- 安装插件后点击左下角绿色的图表，从弹出来的选项中选择 Connect to WSL，稍等片刻即可连接到WSL环境。

- 默认打开的是HOME目录，在菜单栏选择File->Open Folder（快捷键Ctrl+O），再选择自己需要打开的目录。与在Windows宿主机上本地开发体验一样

安装好WSL和VS Code并做好配置后，在WSL中直接运行`code xx`会直接运行VS Code打开某个文件或目录，与在Windows上启动并打开某个目录是一样的效果。更详细的说明请参考：[Remote development in WSL](https://code.visualstudio.com/docs/remote/wsl-tutorial)。

为了更好地支持C语言高亮、自动补全等功能，还需安装如下的`C/C++`插件，根据名字搜索点击安装即可。
![alt text](../../assets/lab2-image-4.png)

## 4. openblas安装

使用预编译的方式直接安装

```text
sudo apt install libopenblas-dev

```

安装后主要新增了头文件和动态链接库，可以确认以下文件是否存在。代码用到openblas，编译的时候需加`-lopenblas`选项，告诉gcc链接到openblas动态库。

```text
/usr/include/x86_64-linux-gnu/cblas.h
/usr/lib/x86_64-linux-gnu/libcblas.so.3.10.3

```

## 5. 实验代码下载

服务器 [http://10.249.12.89:9000/](http://10.249.12.89:9000/) 上的文件，每个实验一个目录，该目录下包含课件和代码，为方便下载，如该实验有多个示例代码，会同时提供zip压缩包。比如实验2的目录`2-lab-naive-gemm`下有code目录和code.zip文件，code.zip是code目录的压缩包，在浏览器中点击文件可以直接下载单个文件。要下载到linux环境，则右键文件，选择“复制链接地址”，使用“wget + 链接”的方式下载，比如使用以下命令下载code.zip

```text
wget http://10.249.12.89:9000/2-lab2-naive-gemm/code.zip

```

下载后使用unzip命令解压到当前目录，比如unzip code.zip。

## 6. 实验报告要求

提交代码和实验报告。

- 课程实验所有要求提交的代码，只提交自己写的或者有做改动的代码文件，其他未做改动的框架代码无需提交。

- 需要贴运行结果的，可以用截图，也可以直接从终端复制文本

- 报告内容如下：

```text
课程信息：高性能计算应用实践、2025秋
实验名称：              上课地点：T2612
姓名：      学号：      班级： 

1、 列出实验环境：OS版本，gcc版本，CPU（型号、频率、物理核数），内存大小
    - `uname –a` 查看操作系统内核版本， `lsb_release –a` 查看发行版本
    - lscpu 查看CPU信息，如果没有频率信息可以通过`cat /proc/cpuinfo` 等方式查看
    - free -h 查看内存
    - 以上命令输出不要截图，直接复制文本，注意格式排版

2、将test_cblas_dgemm.c 修改为行主序，输入矩阵不变，ld{a,b,c}做对应调整，其他参数不变，计算结果与列主序相同还是不同？为什么？

3、time_dgemm.c 分别测试M=N=K：4，16，256，1024，4096，8192时，以下面表格的形式记录两者的运行时长duration和 gflops的值，并对数据做分析说明。
   至少跑到1024。规模4096、8192太大运行时间太长，naive方法用估算时间，openblas用实际运行时间。数据有波动正常，可以运行多次取平均。

|                     | 4     | 16    | 256   | 1024  | 4096  |8192  |
| :---                |  ---: |  ---: |  ---: |  ---: |  ---: | ---: |
| cblas_dgemm duration|       |       |       |       |       |      |
| naive_dgemm duration|       |       |       |       |       |      |
| cblas_dgemm gflops  |       |       |       |       |       |      |
| naive_dgemm gflops  |       |       |       |       |       |      |

4、碰到的问题及解决办法。养成及时记录的习惯，后续所有的实验都可以记录并写入报告。

5、回答以下问题：对于矩阵A、B、C的内存分配，如果使用全局变量在main函数外直接定义，而不用malloc分配，当矩阵规模是8192或者更大的时候，会出现什么错误？
   请修改代码给出实验结果，并尝试回答分析为什么。

```
