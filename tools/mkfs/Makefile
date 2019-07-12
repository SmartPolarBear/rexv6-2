TOP_SRCDIR = ../..
include $(TOP_SRCDIR)/Makefile.common

OUTDIR = $(BUILDDIR)/tools

OBJS = mkfs.o \
	sect.o \

_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))

all: $(OUTDIR)/mkfs

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	$(CC) $^ -iquote $(TOP_SRCDIR)/include -Wextra -Wall -O2 -c -o $@

$(OUTDIR)/mkfs: $(_OBJS)
	$(CC) $(_OBJS) -o $@ 
	
clean:
	rm -f $(OUTDIR)/*

$(OUTDIR):
	mkdir -p $(OUTDIR)

.PHONY: all clean