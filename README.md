# Rexv6
### XV6 modified by XKZ
The xv6 operation system, officially hosted in [mit-pdos/xv6-public](https://github.com/mit-pdos/xv6-public), provided by the course [6.828](https://pdos.csail.mit.edu/6.828/), is a well-known and novel operation system for learners.  

## Goals
- (*)Lottery Scheduling
- (*)Signals  
- (*)Buddy allocator
- VFS
- FAT32 Support  

The (*) tasks has been completed.  

*(To be continued)*  

## Build
Type :  
> make qemu  

Toolchain version:
>$ gcc --version  
>gcc (Debian 6.3.0-18+deb9u1) 6.3.0 20170516  
>Copyright (C) 2016 Free Software Foundation, Inc.  
>This is free software; see the source for copying conditions.  There is NO  
>warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  


## Using C++
The rexv6 project follows the same principle as the zircon kernel in using  C++ shown below.  
> A subset of the C++14 language is used in the Zircon tree. This includes both the upper layers of the kernel (above the lk layer), as well as some userspace code. In particular, Zircon does not use the C++ standard library, and many language features are not used or allowed.
>
> Language features
>
> ​	 **Not allowed**
>
> - Exceptions
> - RTTI and `dynamic_cast`
> - Operator overloading
> - Default parameters
> - Virtual inheritance
> - Statically constructed objects
> - Trailing return type syntax
> - Exception: when necessary for lambdas with otherwise unutterable return types
> - Initializer lists
> - `thread_local` in kernel code
> **Allowed**
> - Pure interface inheritance
> - Lambdas
> - `constexpr`
> - `nullptr`
> - `enum class`es
> - `template`s
> - Plain old classes
> - `auto`
> - Multiple implementation inheritance
> - But be judicious. This is used widely for e.g. intrusive container mixins.
> - Needs more ruling TODO(cpu)
> - Global constructors
> - Currently we have these for global data structures.
>

## XV6 License

> xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
> Version 6 (v6).  xv6 loosely follows the structure and style of v6,
> but is implemented for a modern x86-based multiprocessor using ANSI C.
>
> ACKNOWLEDGMENTS
>
> xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
> to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
> 2000)). See also http://pdos.csail.mit.edu/6.828/2016/xv6.html, which
> provides pointers to on-line resources for v6.
>
> xv6 borrows code from the following sources:
>     JOS (asm.h, elf.h, mmu.h, bootasm.S, ide.c, console.c, and others)
>     Plan 9 (entryother.S, mp.h, mp.c, lapic.c)
>     FreeBSD (ioapic.c)
>     NetBSD (console.c) 
>
> The following people have made contributions: Russ Cox (context switching,
> locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin
> Clements.
>
> We are also grateful for the bug reports and patches contributed by Silas
> Boyd-Wickizer, Cody Cutler, Mike CAT, Nelson Elhage, Nathaniel Filardo, Peter
> Froehlich, Yakir Goaron, Shivam Handa, Bryan Henry, Jim Huang, Anders Kaseorg,
> kehao95, Wolfgang Keller, Eddie Kohler, Imbar Marinescu, Yandong Mao, Hitoshi
> Mitake, Carmi Merimovich, Joel Nider, Greg Price, Ayan Shafqat, Eldar Sehayek,
> Yongming Shen, Cam Tenny, Rafael Ubal, Warren Toomey, Stephen Tu, Pablo Ventura,
> Xi Wang, Keiichi Watanabe, Nicolas Wolovick, Jindong Zhang, and Zou Chang Wei.
>
> The code in the files that constitute xv6 is
> Copyright 2006-2016 Frans Kaashoek, Robert Morris, and Russ Cox.
>
> ERROR REPORTS
>
> If you spot errors or have suggestions for improvement, please send email to
> Frans Kaashoek and Robert Morris (kaashoek,rtm@csail.mit.edu).  If you have
> suggestions for improvements, please keep in mind that the main purpose of xv6
> is as a teaching operating system for MIT's 6.828. For example, we are in
> particular interested in simplifications and clarifications, instead of
> suggestions for new systems calls, more portability, etc.
>
> BUILDING AND RUNNING XV6
>
> To build xv6 on an x86 ELF machine (like Linux or FreeBSD), run "make".
> On non-x86 or non-ELF machines (like OS X, even on x86), you will
> need to install a cross-compiler gcc suite capable of producing x86 ELF
> binaries.  See http://pdos.csail.mit.edu/6.828/2016/tools.html.
> Then run "make TOOLPREFIX=i386-jos-elf-".
>
> To run xv6, install the QEMU PC simulators.  To run in QEMU, run "make qemu".
>
> To create a typeset version of the code, run "make xv6.pdf".  This
> requires the "mpage" utility.  See http://www.mesa.nl/pub/mpage/.



## Acknowledgements
[sunziping2016/xv6-improved](https://github.com/sunziping2016/xv6-improved)  
[david50407/xv6-rpi](https://github.com/david50407/xv6-rpi/tree/master/src)
[avaiyang/xv6-lottery-scheduling](https://github.com/avaiyang/xv6-lottery-scheduling)  
[XV6 Chinese Documents](https://github.com/ranxian/xv6-chinese)    