ifndef TOP_SRCDIR
    TOP_SRCDIR = ..
endif
include $(TOP_SRCDIR)/Makefile.mk

LIBDIR=$(TOP_SRCDIR)/build/lib
ULIBDIR=$(LIBDIR)/ulib
ULIBCDIR=$(LIBDIR)/ulibc

_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))

all:$(TARGET)

$(TARGET): $(_OBJS) $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	$(OBJDUMP) -S $@ > $(OUTDIR)/$(TARGETNAME).asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(OUTDIR)/$(TARGETNAME).sym

clean:
	@for o in $(_OBJS); do echo "REMOVE" $$o; rm -f $$o; done
	@rm -f $(OUTDIR)/*

.PHONY: all clean

include $(TOP_SRCDIR)/Makefile.common.mk