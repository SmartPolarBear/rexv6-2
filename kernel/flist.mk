
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
	xfs.o\
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
	buddy.o\

_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))
