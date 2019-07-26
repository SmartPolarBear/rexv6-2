
ifndef TOP_SRCDIR
    TOP_SRCDIR = .
endif

INCLDIR=$(TOP_SRCDIR)/include
BUILDDIR=$(TOP_SRCDIR)/build

CC = gcc

OBJCOPY = objcopy
OBJDUMP = objdump
RANLIB = ranlib
QEMU = qemu-system-i386.exe

CFLAGS += -nostdinc -fno-pic -fno-builtin -fno-stack-protector -std=gnu11 -Wall -Wextra -m32 -I$(INCLDIR)
CFLAGS += -fno-pie -static -fno-strict-aliasing -fno-omit-frame-pointer
# don't optimize for most code in Debug mode
#CFLAGS += -O2

CFLAGS +=  -fvar-tracking-assignments -g -ggdb 

ASFLAGS += -g -m32 -Wa,-divide -nostdinc -I$(INCLDIR)
LDFLAGS += -m elf_i386

# Debug Settings
GDBPORT = 26000

# Emulation Settings
CPUS = 4
RAM = 4096
