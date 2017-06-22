# 计划
先说说我的几个计划制定的 Guideline 吧。这样大家好理解我为什么那么决定。

首先，我个人认为**我们的目标不在于重新设计轮子，而在于重新制造轮子**。因而以下的所有计划基本都有现成的实现。
总体而言，这些已有的设计是优雅的，经得起时间考验的。在此，也希望大家在实现时不要太过草率，多多参考类 Unix 操作系统的设计，
以及 POSIX 标准。

其次，我觉得这是**操作系统课，而不是软件设计概论**。换言之，我们该做的是与操作系统相关的。所谓操作系统，是与进程调度、
内存管理、IO 等等相关。我们应当提供更丰富的系统调用接口、并提高系统资源分配的效率。而不是把精力花在一堆用户态的奇怪应用上。

总而言之，我的终极目标在于**实现一个 POSIX 操作系统**。POSIX 是一个标准，包含了对系统 API、shell 和其相关组件以及文件系统的规定。
因此，我的初步计划在于重新实现与 Linux 内核保持高度相似的系统调用接口。在系统调用完成的基础上，实现（或适配已有的开源实现）标准 C 语言库函数，
进而讲现有的开源软件顺利的移植到 xv6 平台上。

以下几点需要额外注意：
1. **POSIX 标准没有规定图形界面接口**。且在我看来图形界面没那么重要。所以我可能并不会投入很大精力去帮助图形界面的开发。
2. 在实现了 POSIX 中的系统 API 后。这意味着许多开源应用，将可以原封不动地编译到 xv6 上。这正符合了 POSIX 的 **P**ortable 的初衷。


以下是可以参考以下链接：
* [Linux 系统调用列表](http://syscalls.kernelgrok.com/)
* [Posix 标准](http://pubs.opengroup.org/onlinepubs/9699919799/)
* [Github 上一个增加了标准 C 库的 xv6](https://github.com/DoctorWkt/xv6-freebsd)。根据作者所说，他主要移植了 FreeBSD 1.1.5.1 的标准
C 库代码。

以下是我暂时想到的现在可以改进的地方。

## 标准 C 库移植
注意：由于 xv6 是个独立的操作系统，尚未完整实现标准 C 库。因而，理论上需要链接的头文件一律不能使用（非模板，非 inline 的函数调用）。因而在编译时，
除了`tools`下作为辅助开发的工具，别的地方是没有添加标准 C 库的路径的。

注意：开发操作系统，我希望尽量减少生成代码的不可控性。因而，原则上内核的代码不应当使用 C++，更不能使用 new/delete，异常处理、类（派生、继承）
以及运行时类型识别。至于用户态的代码，还可再做通融。

xv6 本身只实现了少数的标准 C 库函数，我将这部分放在了`distrib/ulib`目录下。标准 C 库实现上的移植工作可以为我们后续开发打下基础。具体操作上，
移植的 C 库头文件应该放在`include`下，需要实现的函数可以在`distrib`独立开个文件。移植的过程可以参照 xv6-freebsd。

## 文件系统
xv6 系统将文件系统的挂载，写死在了内核层面。因而也就无法实现挂载新的设备。这里我希望首先实现 sys_mount 和 sys_umount 两个系统调用。
当然这也需要设备文件组能够将 IDE 设备暴露成块文件。此外，还希望能够对 xv6 的文件系统提供更好支持。包括权限（“用户、组内、其他”式的经典 Unix 权限分类）
日期等等的元信息支持。

此外，我也很希望能够有人支持一下 Fat 文件系统。由于目前 xv6 使用的文件系统并不被主流 Linux 支持，给我们的调试带来了不便。
如果能实现 Fat 之类的文件系统，这样对于我们之后的开发是很方便的。Fat 文件系统相对而言实现较为简单。

## Devfs 设备文件
Unix 的设计哲学之一是**一切皆文件**。通过将各种功能抽象为文件的输入输出。操作系统的 API 就在简化的同时具有了高度的灵活性。xv6
上目前唯一一个设备文件就是`/console`，是一种字符设备，对应于我们 shell 的输入输出。

实际上，Unix 有很多的设备文件，包括磁盘、标准输入输出、`/dev/null`、`/dev/zero`、随机数生成等等。这部分的实现可以（虽然并不必须，但很合理）
放入内核中。具体可参照 Wikipedia 上的 [Device file](https://en.wikipedia.org/wiki/Device_file)。也应当注意 POSIX
[Directory Structure and Devices](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap10.html), 对于设备文件有要求。
实现的设备文件应当出现在`/dev`目录。

## Procfs 虚拟文件系统
可以参照维基百科上的[Procfs](https://en.wikipedia.org/wiki/Procfs)。这里的实现可以仿造 Linux。实际上大家用的 Ubuntu 就是 Linux
发行版之一，可以查看它的`/proc`目录。`/proc`目录的意义在于，将系统的运行状态，诸如 CPU、内存、进程等等的信息，
以文件的方式提供给用户。这也使得操作的 API 更加精简。Proc 应当作为单独的文件系统出现在`/proc`目录。设计的 API 应当与文件系统的 API
保持高度相似。

## 信号
注意：信号（Signal）不是信号量（Semaphore）。不是我们上课讲到的同步机制。

信号作为最基础的 POSIX 进程间通信的手段，虽然很古老很简陋，但却是也很重要。标准 C 库也有对于信号处理方面的涉及`<signal.h>`。
至少有一个功能是我迫切想要的，就是摁下 Ctrl-C，能使当前 Shell 执行的命令停下。这便是信号的作用之一。可以在维基百科那里看到关于信号的一些介绍
[Unix Signal](https://en.wikipedia.org/wiki/Unix_signal)。

## Socket
大家可以参照[Posix Socket](http://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_10)标准。
套接字是网络通信的最基本组件，也是进程间通信最重要的手段之一，相比管道拥有更高的灵活性。

学长在这方面有很多相关的努力。这里可以参考下。具体实现而言，我们不必特别强调网络通信，[Unix domain socket](https://en.wikipedia.org/wiki/Unix_domain_socket)
是一种负担起 Socket 通信的特殊文件。我想实现这类的也算是 Socket 通信。


## 多线程
这方面可以参照[Posix Threads](http://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_09)标准。
这里除了多线程本身外，更重要地是实现线程间同步机制，包括信号量、互斥锁等等。多线程要求线程间有独立的运行栈，这部分可能会涉及到一些比较底层的 C
语言代码。但不论如何，我会尽可能提供必要的支持。

## Shell 及相关组建的移植
开源界有很多的 Shell 及各种组件的实现。比较经典的实现包括 [BusyBox](https://busybox.net/) 和 GNU 的 coreutils。这里希望大家能够将这些组件移植到 xv6 平台上。
似的 Shell 的功能更加强大。此外还可以参照 xv6-freebsd 上的一些实现。

# 组织
我们这次作业会使用 GitHub 进行代码的组织。这里 Git 我只对组长（或组内某一成员）要求，后续我会介绍如何使用。

之后我也会更详细地介绍一下如何实现、开工以及环境配置。


# 目录结构
我整理了 xv6 的各个文件，将其按功能的不同，分配到所属的目录中，并也拆分了 Makefile。
* `boot`: 操作系统引导器。**功能已经完善，无需修改**
* `build`: 编译前为空，存放编译结果
    * `build/fs.img`: 文件系统镜像（由`build/fs`文件夹经`tools/mkfs`打包而成）
    * `build/xv86.img`: 引导及内核
* `distrib`: 用户态程序及相关组件
    * `distrib/ulib`: 常用的库函数、及系统调用接口（静态链接至程序）
    * `distrib/uprogs`: 用户态程序，包括 shell 和常用的命令
* `include`: 所有头文件
    * `include/xv6`: 与操作系统紧密相关的 API
* `kernel`: 内核
* `tools`: 开发工具，目前仅有文件系统镜像打包

# 配置
运行以下命令可以编译并运行。

    make qemu

可以在 `Makefile.common` 中配置编译的 Release/Debug 选项，调试的端口以及模拟器的性能。
默认为 Debug 编译，调试端口为 26000。可运行 gdb，输入如下命令进行远程调试。

    target remote localhost:26000
    file kernel

CLion 支持远程 gdb 调试，效果很好。