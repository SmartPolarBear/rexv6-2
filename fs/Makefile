TOP_SRCDIR = ..
include $(TOP_SRCDIR)/Makefile.mk

OUTDIR=$(BUILDDIR)/fs

SUBDIRS = sfs

OBJS = pipe.o

_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))

all: $(SUBDIRS) $(_OBJS)

.PHONY: all $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MFLAGS) all

clean: 
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done
	@for o in $(_OBJS); do echo "REMOVE" $$o; rm -f $$o; done
	@rm -f $(OUTDIR)/*.d
	@rm -f $(OUTDIR)/*.asm
	@rm -f $(OUTDIR)/*.out

.PHONY: clean

include $(TOP_SRCDIR)/Makefile.common.mk