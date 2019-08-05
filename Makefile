TOP_SRCDIR = .
include $(TOP_SRCDIR)/Makefile.mk	
SETSDIR=$(TOP_SRCDIR)/distrib/sets

SUBDIRS = tools lib kern fs drivers boot bin

BASELIST = $(shell cat $(SETSDIR)/base.list)
OBJS = $(addprefix $(BUILDDIR)/,$(BASELIST))

BASEBINLIST = $(shell cat $(SETSDIR)/base-bin.list)
BASEBINOBJS = $(addprefix $(BUILDDIR)/,$(BASEBINLIST))

BINLIST = $(shell cat $(SETSDIR)/bin.list)
BINOBJS =  $(addprefix $(BUILDDIR)/bin/,$(BINLIST))

$(BUILDDIR)/kernel: $(SUBDIRS) $(OBJS) $(BASEBINOBJS) kern/kernel.ld
	$(LD) $(LDFLAGS) -T kern/kernel.ld -o $@ $(OBJS) -b binary $(BASEBINOBJS)
	$(OBJDUMP) -S $@ > $(BUILDDIR)/kernel.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(BUILDDIR)/kernel.sym

.PHONY: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MFLAGS) all

tags: $(OBJS) kern/init/entryother.S bin/init/init
	etags *.S *.c

# kernelmemfs is a copy of kernel that maintains the
# disk image in memory instead of writing to a disk.
# This is not so useful for testing persistent storage or
# exploring disk buffering implementations, but it is
# great for testing the kernel on real hardware without
# needing a scratch disk.
MEMFSOBJS = $(filter-out $(BUILDDIR)/drivers/ide.o,$(OBJS)) $(BUILDDIR)/drivers/memide.o
$(BUILDDIR)/kernelmemfs: $(MEMFSOBJS) $(BASEBINOBJS) kern/kernel.ld fs.img
	$(LD) $(LDFLAGS) -T kernel.ld -o $ $(MEMFSOBJS) -b binary $(BASEBINOBJS) fs.img
	$(OBJDUMP) -S $(BUILDDIR)/kernelmemfs > $(BUILDDIR)/kernelmemfs.asm
	$(OBJDUMP) -t $(BUILDDIR)/kernelmemfs | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(BUILDDIR)/kernelmemfs.sym


xv6.img:  $(BUILDDIR)/boot/bootblock $(BUILDDIR)/kernel
	dd if=/dev/zero of=xv6.img count=10000
	dd if=$(BUILDDIR)/boot/bootblock of=xv6.img conv=notrunc
	dd if=$(BUILDDIR)/kernel of=xv6.img seek=1 conv=notrunc

xv6memfs.img: $(BUILDDIR)/bootblock  $(BUILDDIR)/kernelmemfs
	dd if=/dev/zero of=xv6memfs.img count=10000
	dd if=$(BUILDDIR)/bootblock of=xv6memfs.img conv=notrunc
	dd if= $(BUILDDIR)/kernelmemfs of=xv6memfs.img seek=1 conv=notrunc


# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
# .PRECIOUS: %.o

fs.img: $(BUILDDIR)/tools/mkfs/mkfs README $(BINOBJS)
	./$< fs.img README $(BINOBJS)

-include *.d

clean: 
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*.o *.d *.asm *.sym vectors.S bootblock entryother \
	initcode initcode.out kernel xv6.img fs.img kernelmemfs \
	xv6memfs.img mkfs .gdbinit \
	$(BINOBJS)

# make a printout
FILES = $(shell grep -v '^\#' $(MISCDIR)/runoff.list)
PRINT =  $(MISCDIR)/runoff.list  $(MISCDIR)/runoff.spec README t $(MISCDIR)/oc.hdr  $(MISCDIR)/toc.ftr $(FILES)

xv6.pdf: $(PRINT)
	./runoff
	ls -l xv6.pdf

print: xv6.pdf

# run in emulators

bochs : fs.img xv6.img
	if [ ! -e .bochsrc ]; then ln -s dot-bochsrc .bochsrc; fi
	bochs -q

qemu: fs.img xv6.img
	$(QEMU) -serial mon:stdio $(QEMUOPTS)

qemu-memfs: xv6memfs.img
	$(QEMU) -drive file=xv6memfs.img,index=0,media=disk,format=raw -smp $(CPUS) -m 256

qemu-nox: fs.img xv6.img
	$(QEMU) -nographic $(QEMUOPTS)

.gdbinit: .gdbinit.tmpl
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

qemu-gdb: fs.img xv6.img .gdbinit
	@echo "*** Now run 'gdb'." 1>&2
	$(QEMU) -serial mon:stdio $(QEMUOPTS) -S $(QEMUGDB)

qemu-nox-gdb: fs.img xv6.img .gdbinit
	@echo "*** Now run 'gdb'." 1>&2
	$(QEMU) -nographic $(QEMUOPTS) -S $(QEMUGDB)

# CUT HERE
# prepare dist for students
# after running make dist, probably want to
# rename it to rev0 or rev1 or so on and then
# check in that version.

EXTRA=\
	mkfs.c ulib.c user.h cat.c echo.c forktest.c grep.c kill.c\
	ln.c ls.c mkdir.c rm.c stressfs.c usertests.c wc.c zombie.c\
	printf.c umalloc.c\
	README dot-bochsrc *.pl toc.* runoff runoff1 runoff.list\
	.gdbinit.tmpl gdbutil\

dist:
	rm -rf dist
	mkdir dist
	for i in $(FILES); \
	do \
		grep -v PAGEBREAK $$i >dist/$$i; \
	done
	sed '/CUT HERE/,$$d' Makefile >dist/Makefile
	echo >dist/runoff.spec
	cp $(EXTRA) dist

dist-test:
	rm -rf dist
	make dist
	rm -rf dist-test
	mkdir dist-test
	cp dist/* dist-test
	cd dist-test; $(MAKE) print
	cd dist-test; $(MAKE) bochs || true
	cd dist-test; $(MAKE) qemu

# update this rule (change rev#) when it is time to
# make a new revision.
tar:
	rm -rf /tmp/xv6
	mkdir -p /tmp/xv6
	cp dist/* dist/.gdbinit.tmpl /tmp/xv6
	(cd /tmp; tar cf - xv6) | gzip >xv6-rev10.tar.gz  # the next one will be 10 (9/17)

$(BUILDDIR):
	mkdir -p $@

.PHONY: dist-test dist
