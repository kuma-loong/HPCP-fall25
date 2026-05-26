# 实验9:基于鲲鹏处理器的性能测试和矩阵乘法优化

## 1. 实验目的

- 了解ARM指令集，了解国产鲲鹏处理器的特点、软件生态

- 进行鲲鹏处理器的理论峰值性能测试

- 将X86应用迁移到ARM，掌握基于ARM平台的矩阵乘法优化

## 2. 实验内容

- 鲲鹏服务器HPL测试

- 

将前述基于框架代码`how-to-optimize-gemm`的naive、pthread、openmp、openblas的实现迁移到华为云的鲲鹏服务器平台，记录实验数据并做分析。

## 3. 华为云资源申请与访问

本实验需要在华为云上使用鲲鹏服务器完成，因华为云代金券有限，限定20个名额，根据加入的顺序发放。未获得代金券的同学，使用华为云公用虚拟机，私聊老师建立账号，请在各自home目录下完成实验。

### 3.1 云资源申请

- 注册账并实名认证加入班级：[https://edu.hicomputing.huawei.com/](https://edu.hicomputing.huawei.com/) ， 等待代金券的发放

- 注册并实名认证华为云：[https://www.huaweicloud.com/](https://www.huaweicloud.com/) ，确认账号有代金券后再进行资源申请。

- 

购买华为云实验资源：参考pdf文档，整体流程相同，部分UI界面与官方平台不同。购买操作的重要说明如下，其他选择默认的配置

  - 选择“自定义购买”，“按需计费”

  - 规格类型选择：鲲鹏计算，实例筛选选择4vCPUs，2Gib，规格选择“鲲鹏通用计算增强型kx1”。

  - 操作系统：公共镜像选择Ubuntu，下来选择Ubuntu 24.04 server 64bit with ARM

  - 取消勾选“开启主机安全防护”、“开启备份”

  - 公网IP使用默认配置，选择按流量计费，5Mbit/s

  - 登录凭证，设置密码，账户是root

  - 最终的配置概要确认如下，费用为：¥0.1588/小时+弹性公网IP流量费用¥0.80/GB
![alt text](../../assets/lab9-imgs/image.png)

  - 勾选协议，再点击立即购买。

  - 购买后会进入实例创建状态，等待几分钟，创建成功后会启动进入运行中

  - 启动后点击重置密码，重置密码需要重启才会生效。

![alt text](../../assets/lab9-imgs/image-7.png)

按需计费是用多少算多少，实例释放后会自动从代金券抵扣，可在“费用”模块查看扣费详单。注意代金券有使用范围限制，其他非教学相关使用造成的费用自行负责。

### 3.2 SSH登录远程服务器

SSH 是 Secure Shell 的缩写，是一种网络协议，通过对网络数据进行加密和验证，在不安全的网络环境（比如互联网）中安全地进行远程登录和数据传输。统远程登录和文件传输方式，例如Telnet、FTP，使用明文传输数据，存在很多的安全隐患。

使用ssh登录远程服务器，在终端输入以下`ssh root@113.44.221.187`，IP地址替换为自己实例的公网IP，192.168.开头的是局域网IP。

```text
 $ ~ »ssh root@113.44.221.187 
The authenticity of host '113.44.221.187 (113.44.221.187)' can't be established.
ECDSA key fingerprint is SHA256:roYRjPCuKLy2x3QmSxs7gJ5YHNC+wJSjFgqWU2r42Vo.
Are you sure you want to continue connecting (yes/no/[fingerprint])?

```

输入后按回车，会有一个提示，输入yes继续，接着输入密码，验证通过后登录到远端服务器，剩下的操作跟WSL的操作是一样的。公共镜像已经将apt的源修改为了华为自己的镜像源，无需再修改。

## 4. 性能测试

### 4.1 峰值性能测试:cpufp

鲲鹏服务器用的CPU是ARM架构，不同于大部分同学笔记本用的X86架构（苹果电脑2020年开始用ARM架构的处理器），如果直接将X86平台的Linux可执行文件cpufp拷贝到鲲鹏服务器，无法被运行。2025年发布的鸿蒙电脑，也是使用ARM架构的CPU而不再使用Intel或AMD的X86架构CPU。

根据cpufp的说明进行ARM平台的编译，运行结果中Instruction Set是asimd，Core Computation 是mla.vs(f64,f64,f64)的结果是最大的双精度浮点峰值性能。cpufp在鲲鹏服务器上的运行不做要求。

### 4.2 HPL测试

修改配置，重新编译，生成xhpl可执行文件，根据硬件配置修改HPL.dat，记录HPL结果。 

鲲鹏服务器上mpirun 指定多个进程会报错，将PQ都改为1，直接./xhpl启动，会自动开启CPU核数对应的线程数，可以top查看cpu的利用率确认是开启了多线程在跑。

## 5. 鲲鹏服务器矩阵乘法优化

涉及到的工具、openblas库在ARM平台都有比较好的支持，操作基本不变。将代码拷贝到远端服务器，编译运行对应的代码即可，以下命令都是在本地的WSL的终端下执行。

将本地文件或目录拷贝到远端服务器：

- 拷贝文件：scp {{path/to/local_file}} {{remote_host}}:{{path/to/remote_file}}

  - 比如：`scp how-to-optimize-gemm/makefile root@ip:~`

- 拷贝目录：scp -r {{path/to/local_directory}} {{remote_host}}:{{path/to/remote_directory}}

  - 比如：`scp -r how-to-optimize-gemm root@ip:~`

将远端服务器文件或目录拷贝到本地：

- 拷贝文件：scp {{remote_host}}:{{path/to/remote_file}} {{path/to/local_directory}}

  - 比如：`scp root@ip:~/makeifle ./`

- 拷贝目录：scp -r {{remote_host}}:{{path/to/remote_directory}} {{path/to/local_directory}}

  - 比如：`scp -r root@ip:~/how-to-optimize-gemm ./`

## 6. 删除资源

实验后及时删除弹性云服务器，否则将不断产生费用代金券无法覆盖。若开了主机安全和云备份，记得删除释放。在控制台的总览可以看到账号下的资源情况。

![alt text](../../assets/lab9-imgs/image-8.png)![alt text](../../assets/lab9-imgs/image-1.png)

## 7. 附加题（1分）

在鲲鹏服务器上编译安装华为高性能通信库（Hyper MPI）进行HPL的测试，并与非Hyper MPI库进行对比。

Hyper MPI是基于OpenMPI 4.1.1和UCX 1.10.1，支持MPI-V3.1标准的并行计算API接口，新增了优化的集合通信框架。同时，Hyper MPI对数据密集型和高性能计算提供了网络加速能力，使能了节点间高速通信网络和节点内共享内存机制，以及优化的集合通信算法。

安装方式请参考官方手册：[https://support.huawei.com/enterprise/en/doc/EDOC1100318949/8c564679/installing-hyper-mpi-using-source-code-open-source-components](https://support.huawei.com/enterprise/en/doc/EDOC1100318949/8c564679/installing-hyper-mpi-using-source-code-open-source-components)

## 8. 实验报告要求

- 提交实验报告，报告内容如下：

```text
课程信息：高性能计算应用实践、2025秋
实验名称：              上课地点：T2612
姓名：      学号：      班级： 

1、lscpu输出

2、CPU理论浮点峰值性能计算过程和结果
   lscpu没有鲲鹏处理器的频率信息，可以直接用2.5GHz计算
   参考https://www.hisilicon.com/cn/products/kunpeng/huawei-kunpeng/huawei-kunpeng-920

3、cpufp测试结果

4、HPL的测试结果
  - 如做附加题，分别列出Hyper MPI和非Hyper MPI的结果，并给出which mpirun的输出

5、 gflops曲线图
    - 基于框架代码`how-to-optimize-gemm`的naive、pthread、openmp、openblas四种方式得到的数据绘制在同一个图。
    - 画图可以用python plotAllfile.py，会自动读取_data目录下所有.m数据文件（不包括output_new.m和output_old.m）绘图。
    - 矩阵规模至少包括4，16，64，256，512，1024，2048。
    - 如果"矩阵规模/CPU核心数"的结果不是整数，可以调整矩阵规模比如1000，2000。
    - openmp和pthread的线程数设置为CPU物理核心数。

6、碰到的问题及解决方法

```
