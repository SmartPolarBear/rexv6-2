TOP_SRCDIR = ..

$(OUTDIR):
	mkdir -p $@

all: fuck $(OUTDIR)/bootblock

fuck:
	mkdir -p $(OUTDIR)

$(OUTDIR)/bootblock: bootasm.S bootmain.c sign.pl | $(OUTDIR)
	
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -I. -c bootmain.c -o $(OUTDIR)/bootmain.o
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -c bootasm.S -o $(OUTDIR)/bootasm.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o $(OUTDIR)/bootblock.o $(OUTDIR)/bootasm.o $(OUTDIR)/bootmain.o
	$(OBJDUMP) -S $(OUTDIR)/bootblock.o > $(OUTDIR)/bootblock.asm
	$(OBJCOPY) -S -O binary -j .text $(OUTDIR)/bootblock.o $(OUTDIR)/bootblock
	./sign.pl $(OUTDIR)/bootblock

include Makefile.boot.mk

