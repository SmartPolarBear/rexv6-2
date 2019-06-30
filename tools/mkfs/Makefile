TOP_SRCDIR = ../..
include $(TOP_SRCDIR)/Makefile.common

OUTDIR = $(BUILDDIR)/tools

all: $(OUTDIR)/mkfs

$(OUTDIR)/mkfs: mkfs.c | $(OUTDIR)
	$(CC) -Wextra -Wall -O2 -o $@ $^ -iquote $(TOP_SRCDIR)/include
	
clean:
	rm -f $(OUTDIR)/mkfs

$(OUTDIR):
	mkdir -p $(OUTDIR)

.PHONY: all clean