ifndef TOP_SRCDIR
    TOP_SRCDIR = ..
endif
include $(TOP_SRCDIR)/Makefile.mk

OUTDIR=$(BUILDDIR)/kernel

_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))

all:$(_OBJS)

$(OUTDIR)/entryother: entryother.S | $(OUTDIR)
	@echo $< "->" $@
	@$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -c entryother.S -o $(OUTDIR)/entryother.o
	@$(LD) $(LDFLAGS) -N -e start -Ttext 0x7000 -o $(OUTDIR)/bootblockother.o $(OUTDIR)/entryother.o
	@$(OBJCOPY) -S -O binary -j .text $(OUTDIR)/bootblockother.o $(OUTDIR)/entryother
	@$(OBJDUMP) -S $(OUTDIR)/bootblockother.o > $(OUTDIR)/entryother.asm

$(OUTDIR)/initcode: initcode.S | $(OUTDIR)
	@echo $< "->" $@
	@$(CC) $(CFLAGS) -nostdinc -I. -c initcode.S -o $(OUTDIR)/initcode.o
	@$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $(OUTDIR)/initcode.out $(OUTDIR)/initcode.o
	@$(OBJCOPY) -S -O binary $(OUTDIR)/initcode.out $(OUTDIR)/initcode
	@$(OBJDUMP) -S $(OUTDIR)/initcode.o > $(OUTDIR)/initcode.asm

vectors.S: vectors.pl
	"[vectors.pl]" $< "->" $@
	./vectors.pl > vectors.S

$(OUTDIR)/vectors.o: vectors.S | $(OUTDIR)
	echo "[CC]" $< "->" $@
	@$(CC) $(OUTDIR)/vectors.S $(CFLAGS) -c -o $@

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	@echo "[CC]" $< "->" $@
	@($(CC) $< $(CFLAGS) -c -o $@)

$(OUTDIR)/%.o: %.S | $(OUTDIR)
	@echo "[CC]" $< "->" $@
	@($(CC) $< $(CFLAGS) -c -o $@)

$(OUTDIR):
	@echo "MKDIR" $@
	@mkdir -p $@

clean:
	@for o in $(_OBJS); do echo "REMOVE" $$o; rm -f $$o; done
	@rm -f $(OUTDIR)/*.d
	@rm -f $(OUTDIR)/*.asm
	@rm -f $(OUTDIR)/*.out
	@rm -f bootblockother.o


.PHONY: all clean