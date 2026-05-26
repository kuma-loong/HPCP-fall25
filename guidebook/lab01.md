# 实验1:Linux操作系统与shell命令

## 1. 实验目的

- 了解linux/unix内核和发展历史，掌握linux环境安装

- 掌握shell、man等工具的使用

## 2. 实验内容

- 安装WSL并配置运行Ubuntu

- 在Ubuntu中练习常用命令使用，使用shell命令统计指定目录下所有目录数、文件数和所有文件内容的行数。

## 3. WSL下 Linux环境安装

### 3.1 安装步骤

Windows子系统（Windows Subsystem for Linux，WSL）是一个为在Windows 10及以上版本能够原生运行Linux二进制可执行文件（ELF格式）的兼容层。WSL针对不同的windows版本有自动安装和手动安装两种方式，自动安装适用于 Windows 10 版本 2004 及以上（内部版本号是19041），或 Windows 11。

自动安装步骤如下，主要参考：[How to install Linux on Windows with WSL](https://learn.microsoft.com/en-us/windows/wsl/install)

- 步骤1：以管理员身份打开 Windows PowerShell（右击「开始」按钮，选择 Windows PowerShell（管理员)

- 步骤2：输入 `wsl --install`，该命令会打开运行WSL需要的特性开关，以及安装默认的Linux发行版Ubuntu，命令执行完后重启计算机。

- 步骤3：安装完成后，可以在「开始」菜单的标签页中找到安装的Ubuntu。

如果正在使用 Windows10 2004 以下版本或电脑没打开虚拟化，在运行`wsl --install`会出现超时或者无法连接服务器的错误，需按以下步骤手动安装，详细说明请参考：[Manual installation steps for older versions of WSL](https://learn.microsoft.com/en-us/windows/wsl/install-manual)。

- 

步骤1：以管理员身份打开PowerShell，执行以下命令打开wsl选项

`dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart`

- 步骤2：检查系统的版本要求看是否符合WSL 2，`Win+R`打开运行窗口，输入`winver`回车，可以看到内部版本号，大于19041，否则按照手册说明升级windows版本。

- 

步骤3：在PowerShell执行以下命令打开虚拟机选项

`dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart`

- 步骤4：安装linux kernel升级包，先下载[WSL2 Linux kernel update package for x64 machines]（[https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_x64.msi](https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_x64.msi)），再双击打开按照提示安装。

- 步骤5：在PowerShell执行`wsl --set-default-version 2`将WSL默认版本设置为version 2。

- 步骤6：打开Microsoft Store，搜索Ubuntu，按照提示安装。

- 步骤7：安装完后在Windows PowerShell 执行 `wsl -l -v` 查看安装的发行版的版本以及当前的状态。

安装过程可能碰到以下问题

- wsl --install超时，按照[https://oi-wiki.org/tools/wsl](https://oi-wiki.org/tools/wsl)在“程序”中打开wsl子系统选项，然后重启。重启后wsl --install或者wsl -l -v，会提示在microsoft appstore中搜索安装。

- wsl --install 提示找不到，在在“程序”中打开wsl子系统选项。

- wsl --install 连接服务失败，但wsl -l -v会提示microsoft appstore中安装。

- 其他问题需结合错误提示，使用搜索引擎搜索解决方式，如果仍未解决，则使用虚拟机安装，或者使用实验室电脑的环境。

### 3.2 运行、配置Ubuntu

打开windows「开始」菜单找到 Ubuntu 并启动，第一次运行需要几分钟初始化

```text
Installing, this may take a few minutes...

```

等待一两分钟时间，系统会提示创建新的用户帐户，输入你希望创建的用户名，可以与windows系统的用户名相同。

```text
Please create a default UNIX user account. The username does not need to match your Windows username.
For more information visit: https://aka.ms/wslusers
Enter new UNIX username: YourName

```

输入完用户名以后会提示输入密码。在 Linux 中，输入密码时屏幕上默认不显示输入的内容即关闭回显。在输入密码的场景，出于安全考虑默认都是关闭回显。

```text
Enter new UNIX password:

```

设置好帐户名和密码后，Ubuntu就初始化完成了。

```text
Installation successful!
To run a command as administrator (user "root"), use "sudo <command>".
See "man sudo_root" for details.

YourName@SENIORIOUS:~$

```

初次安装好的系统不附带任何 C 编译器，需要手动配置环境。

```text
$ gcc
The program 'gcc' is currently not installed. You can install it by typing:
sudo apt install gcc

```

Ubuntu等Linux系统很多常用的软件可以直接在线安装，软件文件存在远端服务器上，为了加速软件安装，很多公司或者组织会搭建一个服务器，该服务器会同步官方服务器的数据，叫做镜像源，加速软件的分发。用户在使用的时候，通常配置一个网速最快的镜像源。这里推荐校内“开源技术协会”搭建的镜像源：[https://mirrors.osa.moe](https://mirrors.osa.moe)， 该站点只能校园网访问。

对于Ubuntu的软件仓库镜像配置修改请参考：[https://mirrors-help.osa.moe/ubuntu/](https://mirrors-help.osa.moe/ubuntu/)，先选择Ubuntu的版本，然后选择快速配置，复制快速配置的内容到终端运行。

镜像源更新可能碰到的问题：

- 提示没有权限即使加了sudo，sudo只对管道符前面的部分生效，后面的重定向符号的权限并没有获得提升。先给文件添加可写的权限，sudo chmod o+w /etc/apt/sources.list 再执行复制过来的快捷命令。
  

- 校区内镜像源没有Ubuntu 24.04及以后的版本，可使用[南科大镜像源](https://mirrors.sustech.edu.cn/help/ubuntu.html#introduction)等校外镜像源。

修改软件的镜像配置后，先后执行以下两条命令，第一条命令更新本地的应用数据库，第二条命令安装编译环境。

```text
sudo apt update
sudo apt install -y build-essential vim gdb

```

参考：[https://oi-wiki.org/tools/wsl/#%E9%85%8D%E7%BD%AE%E5%88%86%E5%8F%91%E7%89%88](https://oi-wiki.org/tools/wsl/#%E9%85%8D%E7%BD%AE%E5%88%86%E5%8F%91%E7%89%88)

### 3.3 Windows Terminal 安装

Windows 自带的终端界面不太美观，推荐安装Windows Terminal，Windows Terminal 是微软推出的一个现代化终端应用，专门为 Windows 10 及更新版本设计。它是命令行工具的统一入口，支持 PowerShell、命令提示符 (CMD)、WSL (Linux 子系统)、Azure Cloud Shell 等多种 Shell。

具体安装方式参考：[Install and get started setting up Windows Terminal](https://learn.microsoft.com/en-us/windows/terminal/install)。

安装 Windows Terminal 之后，配置终端默认配置为 WSL 安装的 Ubuntu，以后直接启动Windows Terminal即进入Ubuntu。Windows Terminal的颜色、字体等配置根据个人喜好自行配置。
![alt text](../../assets/lab1-image-2.png)

## 4. Linux文件和目录

Linux跟windows一样也有目录和文件的概念，且含义相同。所有的数据都存储在磁盘上面，磁盘通过文件系统进行数据的组织。Linux可以访问windows的数据，windows也可以访问linux的数据。但Linux的文件、目录的访问，与windows有很多不同。Linux下主要是使用命令行，对初学linux的同学，往往不熟悉命令行而觉得不好用，唯有多用多练习，熟练之后会感受到命令行的强大。

### 4.1 Linux目录结构

打开 Windows Terminal 进入终端，执行`tree / -L 1`命令 ，即用tree命令以树状的形式列出根目录下的所有内容。`/`是根目录，是Linux文件系统的入口，每个子目录的作用先不用关注。查看目录下的内容常用的有ls命令。

```text
tree $ ~ »tree / -L 1
/
├── bin
├── boot
├── dev
├── etc
├── home
├── init
├── lib
├── lib32
├── lib64
├── libx32
├── media
├── mnt
├── opt
├── proc
├── root
├── run
├── sbin
├── srv
├── sys
├── tmp
├── usr
├── var

```

### 4.2 绝对路径和相对路径

- 所有路径的字符都是连续的，中间不能有空格，比如`/etc/ssh/ssh_config`是一个绝对路径，如果中间加空格`/etc/ssh/ ssh_config`，则分成了两部分。

- 绝对路径：从根目录开始，写法由根目录 / 写起，例如： /etc/apt 这个目录。

- 相对路径：从当前目录开始，写法不以/开始，例如由 /etc/apt 进入/etc/ssh目录时，可以写成： cd ../ssh。  

  - 两个点`..` 代表当前目录的上一层。

  - 一个点`.`代表当前目录，往往省略掉，比如在/etc目录下运行`cd apt`，表示进入apt目录，跟`cd ./apt`等同。

  - 可以叠加，比如`../..`代表上一级目录的上一级。

- home目录，Linux支持多用户，每个用户在/home路径下都有一个与用户名相同的目录，称为主目录也叫家目录，用`~`表示。对普通用户，在任意路径下，`cd ~`都是回到用户`xx`的`/home/xx`目录下。对于root用户，`～`表示`/root`。

### 4.3 处理目录的常用命令

- ls（list files）: 列出目录及文件名

- cd（change directory）：切换目录

- pwd（print work directory）：显示目前的目录

- mkdir（make directory）：创建一个新的目录

- cp（copy file）: 复制文件或目录

- rm（remove）: 删除文件或目录

- mv（move file）: 移动文件与目录，或修改文件与目录的名称

其中ls、cp、rm、mv不仅可以操作目录，也可以操作文件。

### 4.4 处理文件的常用命令

- ls 还可查看大小、创建时间、权限、绝对路径等，比如`ls -l /etc/passwd`，`-l`选项是使用长列表格式，会列出较多信息，不加则只会列出路径。

- rm 删除文件

- vi 查看或者编辑文件

- cat 显示文件内容

### 4.5 与windows的区别

- 文件系统结构

  - Linux: 使用单一的目录结构，所有的文件和目录都从根目录 `/` 开始。文件系统是一个统一的树状结构，不论文件实际位于哪个磁盘分区，用户都通过统一的目录树访问。例如：`/etc/config`

  - Windows: 使用多个磁盘分区（如 C:、D:、E: 等），并且每个盘符有自己的根目录。文件路径通常以盘符开始，后面跟着文件夹名称。例如：`C:\Users\user\Documents`。

- 路径分隔符

  - Linux: 使用正斜杠 `/` 作为路径分隔符。例如：`/home/user/file.txt`

  - Windows: 使用反斜杠 `\` 作为路径分隔符。例如：`C:\Users\user\file.txt`

- 文件名区分大小写

  - Linux: 区分大小写， `file.txt` 和 `File.txt` 是两个不同的文件。

  - Windows: 不区分大小写， `file.txt` 和 `File.txt` 被认为是同一个文件。

- 系统目录

  - Linux: 系统目录结构有固定的标准：

    - `/etc`：系统配置文件

    - `/bin`：基本命令二进制文件

    - `/home`：用户的主目录

    - `/root`：超级用户（root）的主目录

    - `/var`：存储可变数据（如日志文件）

    - `/usr`：用户级程序和文件

  - Windows: 目录结构更加自由，一些常见的系统目录包括：

    - `C:\Windows`：Windows 系统文件

    - `C:\Program Files`：程序安装目录

    - `C:\Users`：用户的主目录（每个用户一个子目录）

- 隐藏文件

  - Linux: 文件名以点（`.`）开头的文件被视为隐藏文件（如 `.bashrc`）。

  - Windows: 文件属性可以设置为“隐藏”，但通常没有文件名前缀来标识隐藏文件。

## 5. Linux下常用快捷操作

- Tab键命令补全：输入命令的一部分，按tab键补全，如果有多个匹配会都列出来作为提示，接着输剩余的字符完成最终的输入，如果没有匹配的则无法补全。

- 上下方向键查看历史命令：Linux对输入的命令做了保存，可以按上下键调取输入过的命令。

## 6. WSL与Windows互相访问

- 

从 Windows 访问 WSL 文件

可以通过资源管理器直接访问WSL 中的 Linux 文件系统，打开资源管理器，在地址栏输入`\\wsl$`，会列出已安装的所有 Linux 发行版，双击要进入的Linux发行版既可以即可像操作本地文件一样进行复制、粘贴、编辑。

- 

从 WSL 访问 Windows 文件

在 WSL 中，Windows 的磁盘会自动挂载到 /mnt 目录下，例如`ls /mnt/d` 即查看 Windows D盘上的文件。

```text
C: 盘对应 /mnt/c
D: 盘对应 /mnt/d

```

## 7. Linux下文件编辑

Linux下可用的编辑器有很多，有些是系统自带，有些需要安装，主要分为以下几类：

- 命令行编辑器：常见的是vi，以及功能更强的增强版vim（Vi IMproved），Linux 自带，功能强大，学习成本高，有多种模式，对应教程vimtutor。

- 图形界面编辑器：常见的是gedit，简单易用，带语法高亮。

- IDE：VS Code。

- 在Windows中编辑完传给Linux，或者直接在Windows下编辑WSL中的文件，初学者紧急情况下可用，不建议。

## 8. 实验内容：目录和文件统计

使用shell命令统计指定目录下所有目录的数量、文件数量以及所有文件总的行数（每个文件的行数加起来，包括空行），子目录中的文件无需统计。比如/usr/include目录有以下内容（只列出了部分目录和文件），有5个目录，5个文件，文件行数统计只需要统计5个文件的行数。

```text
/usr/include
├── malloc.h
├── math.h
├── mcheck.h
├── memory.h
├── netash
│   └── ash.h
├── netatalk
│   └── at.h
├── netax25
│   └── ax25.h
├── netdb.h
├── neteconet
│   └── ec.h
├── netinet
│   ├── ether.h
│   ├── if_ether.h
│   ├── ip.h
│   ├── ip_icmp.h
│   ├── tcp.h
│   └── udp.h

```

操作方式不限，可以分开用不同的命令实现，输出格式不限，可以自行构造数据测试不必统计/usr/include目录。可能需要用到ls、cat、grep、wc、xargs等命令以及管道。

## 9. 实验报告要求

提交到作业系统：

- 课程实验报告写作工具不限，markdown或word都可以，最终导出pdf提交

- 需要贴运行结果的，可以用截图，也可以直接从终端复制文本

- 报告内容模版如下，实验1仅提交一个pdf文件

```text
  课程信息：高性能计算应用实践、2025秋
  实验名称：              上课地点：T2612
  姓名：      学号：      班级： 

  1、 目录和文件统计的实现
      - 代码：实现的shell命令或者shell脚本的内容，以及实现思路说明
      - 测试结果

  2、碰到的问题及解决方法

```
