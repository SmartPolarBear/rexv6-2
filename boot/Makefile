TOP_SRCDIR = ..
include $(TOP_SRCDIR)/rexv6.mk

OUTDIR = $(BUILDDIR)/boot

CFLAGS2 += -nostdinc -fno-pic -fno-builtin -fno-stack-protector -m32 -I$(INCLDIR)
CFLAGS2 += -fno-pie -fno-strict-aliasing -fno-omit-frame-pointer -Os -s
#CFLAGS2 += -ffunction-sections -fdata-sections -Wl,--gc-sections -fdelete-null-pointer-checks

all: bootblock

bootblock: $(OUTDIR)/bootblock

$(OUTDIR)/bootblock: bootasm.S bootmain.c sign.pl | $(OUTDIR)
	$(CC) $(CFLAGS2) -nostdinc -fno-pic -c bootmain.c -o $(OUTDIR)/bootmain.o
	$(CC) $(CFLAGS2) -nostdinc -fno-pic -c bootasm.S -o $(OUTDIR)/bootasm.o
	$(LD) $(LDFLAGS) -N -T boot.ld -o $(OUTDIR)/bootblock.o $(OUTDIR)/bootasm.o $(OUTDIR)/bootmain.o
	$(OBJDUMP) -S $(OUTDIR)/bootblock.o > $(OUTDIR)/bootblock.asm
	$(OBJCOPY) -S -O binary $(OUTDIR)/bootblock.o $@
	perl ./sign.pl $@

$(OUTDIR):
	mkdir -p $@

clean:
	rm -f $(OUTDIR)/*

.PHONY: all bootblock clean
