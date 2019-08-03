ifndef TOP_SRCDIR
    TOP_SRCDIR = ..
endif
include $(TOP_SRCDIR)/Makefile.mk

OUTDIR=$(BUILDDIR)/boot

$(OUTDIR):
	mkdir -p $@

clean:
	rm -f $(OUTDIR)/*

.PHONY: all clean