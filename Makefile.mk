ifndef TOP_SRCDIR
    TOP_SRCDIR = .
endif

MISCDIR=$(TOP_SRCDIR)/misc
INCDIR=$(TOP_SRCDIR)/include
BUILDDIR=$(TOP_SRCDIR)/build

# Cross-compiling (e.g., on Mac OS X)
# TOOLPREFIX = i386-jos-elf

# Using native tools (e.g., on X86 Linux)
#TOOLPREFIX = i386-elf-

# Try to infer the correct TOOLPREFIX if not set
ifndef TOOLPREFIX
TOOLPREFIX := $(shell if i386-jos-elf-objdump -i 2>&1 | grep '^elf32-i386$$' >/dev/null 2>&1; \
	then echo 'i386-jos-elf-'; \
	elif objdump -i 2>&1 | grep 'elf32-i386' >/dev/null 2>&1; \
	then echo ''; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find an i386-*-elf version of GCC/binutils." 1>&2; \
	echo "*** Is the directory with i386-jos-elf-gcc in your PATH?" 1>&2; \
	echo "*** If your i386-*-elf toolchain is installed with a command" 1>&2; \
	echo "*** prefix other than 'i386-jos-elf-', set your TOOLPREFIX" 1>&2; \
	echo "*** environment variable to that prefix and run 'make' again." 1>&2; \
	echo "*** To turn off this error, run 'gmake TOOLPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

CC = $(TOOLPREFIX)gcc
CXX = $(TOOLPREFIX)g++
AS = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
ASFLAGS = -m32 -gdwarf-2 -Wa,-divide
# FreeBSD ld wants ``elf_i386_fbsd''
LDFLAGS += -m $(shell $(LD) -V | grep elf_i386 2>/dev/null | head -n 1)

COMMONCFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -fno-omit-frame-pointer -I$(INCDIR)
COMMONCFLAGS += -O2
COMMONCFLAGS +=  -m32 

CFLAGS += $(COMMONCFLAGS)
CFLAGS += -std=gnu18
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

CXXFLAGS +=  $(COMMONCFLAGS)
CXXFLAGS += -std=gnu++17 -mno-sse -ffreestanding -mno-red-zone -nostdlib
CXXFLAGS += -fno-builtin -Wall -m32 -fpermissive -fno-stack-protector -fno-exceptions -fno-rtti
CXXFLAGS += $(shell $(CXX) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

HOST_CC = gcc
HOST_CXX = g++
HOST_CFLAGS = -Werror -Wall -std=gnu18 -I$(INCDIR)
HOST_CXXFLAGS = -Werror -Wall -std=gnu++2a -I$(INCDIR)

# try to generate a unique GDB port
GDBPORT = $(shell expr `id -u` % 5000 + 25000)
