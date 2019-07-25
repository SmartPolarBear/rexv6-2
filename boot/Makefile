TOP_SRCDIR = ..
include $(TOP_SRCDIR)/rexv6.mk

OUTDIR = $(BUILDDIR)/boot

CFLAGS2 = -nostdinc -fno-pic -fno-builtin -fno-stack-protector -std=gnu11 -m32 -I$(INCLDIR)
CFLAGS2 += -fno-pie -static -fno-strict-aliasing -fno-omit-frame-pointer
CFLAGS2 += -fvar-tracking-assignments

all: bootblock

bootblock: $(OUTDIR)/bootblock


$(OUTDIR)/bootblock: bootasm.S bootmain.c sign.pl | $(OUTDIR)
	$(CC) -Os $(CFLAGS2) -fno-pic -c bootmain.c -o $(OUTDIR)/bootmain.o
	$(CC) -Os $(CFLAGS2) -fno-pic -c bootasm.S -o $(OUTDIR)/bootasm.o
	$(LD) -Os $(LDFLAGS) -N -e start -Ttext 0x7C00 -o $(OUTDIR)/bootblock.o $(OUTDIR)/bootasm.o $(OUTDIR)/bootmain.o
	$(OBJDUMP) -S $(OUTDIR)/bootblock.o > $(OUTDIR)/bootblock.asm
	$(OBJCOPY) -S -O binary -j .text $(OUTDIR)/bootblock.o $@
	perl ./sign.pl $@

$(OUTDIR):
	mkdir -p $@

clean:
	rm -f $(OUTDIR)/*

.PHONY: all bootblock clean
