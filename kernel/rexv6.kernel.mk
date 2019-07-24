#to complete a makefile for kernel itself
include $(TOP_SRCDIR)/rexv6.mk
OUTDIR = $(BUILDDIR)/kernel

_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))

all:$(_OBJS)

$(OUTDIR)/entryother: entryother.S | $(OUTDIR)
	$(CC) $(CFLAGS) -fno-pic -c entryother.S -o $(OUTDIR)/entryother.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7000 -o $(OUTDIR)/entryother.out $(OUTDIR)/entryother.o
	$(OBJCOPY) -S -O binary -j .text $(OUTDIR)/entryother.out $(OUTDIR)/entryother
	$(OBJDUMP) -S $(OUTDIR)/entryother.out > $(OUTDIR)/entryother.asm

$(OUTDIR)/initcode: initcode.S | $(OUTDIR)
	$(CC) $(CFLAGS) -c initcode.S -o $(OUTDIR)/initcode.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $(OUTDIR)/initcode.out $(OUTDIR)/initcode.o
	$(OBJCOPY) -S -O binary $(OUTDIR)/initcode.out $(OUTDIR)/initcode
	$(OBJDUMP) -S $(OUTDIR)/initcode.o > $(OUTDIR)/initcode.asm

$(OUTDIR)/vectors.o: vectors.pl | $(OUTDIR)
	perl vectors.pl > $(OUTDIR)/vectors.S
	$(CC) $(OUTDIR)/vectors.S $(CFLAGS) -c -o $@

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	$(CC) $< $(CFLAGS) -fno-pic -c -o $@

$(OUTDIR)/%.o: %.S | $(OUTDIR)
	$(CC) $< $(CFLAGS) -fno-pic -c -o $@

$(OUTDIR):
	mkdir -p $@

clean:
	for o in $(_OBJS); do rm -f $$o; done

.PHONY: all kernel clean