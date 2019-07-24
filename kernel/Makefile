TOP_SRCDIR = ..
include $(TOP_SRCDIR)/rexv6.mk
include flist.mk

OUTDIR = $(BUILDDIR)/kernel

SUBDIRS = dev \
	fs \
	init \
	lib \
	mm \
	proc \
	sync \
	syscall \
	trap \

all: kernel

kernel:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir all;	done
	$(MAKE) $(OUTDIR)/kernel

$(OUTDIR)/kernel: $(_OBJS) $(OUTDIR)/entryother $(OUTDIR)/initcode kernel.ld | $(OUTDIR)
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir all;	done
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(_OBJS) -b binary $(OUTDIR)/initcode $(OUTDIR)/entryother
	$(OBJDUMP) -S $@ > $(OUTDIR)/kernel.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(OUTDIR)/kernel.sym

clean:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done
	rm -f $(OUTDIR)/entryother.*
	rm -f $(OUTDIR)/initcode.*

.PHONY: all kernel clean
