TOP_SRCDIR = ..
include $(TOP_SRCDIR)/Makefile.common
OUTDIR = $(BUILDDIR)/kernel

OBJS = \
	bio.o\
	console.o\
	devzero.o\
	devrandom.o\
	devnull.o\
	devfull.o\
	devhda.o\
	sound.o\
	devsound.o\
	devhdainfo.o\
	devperfctr.o\
	entry.o\
	exec.o\
	file.o\
	fs.o\
	ide.o\
	ioapic.o\
	kalloc.o\
	kbd.o\
	lapic.o\
	log.o\
	main.o\
	mount.o\
	mp.o\
	picirq.o\
	pipe.o\
	proc.o\
	sleeplock.o\
	spinlock.o\
	string.o\
	swtch.o\
	syscall.o\
	tui.o\
	sysfile.o\
	sysproc.o\
    syssync.o\
	timer.o\
	trap.o\
	uart.o\
	trapasm.o\
	signalasm.o\
	vectors.o\
	vm.o\
	semaphore.o\
	rwlock.o\
	rand.o\
	qsort.o\
	sysmount.o \

_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))


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
