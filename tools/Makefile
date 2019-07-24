TOP_SRCDIR = ..
include $(TOP_SRCDIR)/rexv6.mk

OUTDIR = $(BUILDDIR)/tools

SUBDIRS = mkfs \

all: 
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir all;	done

clean:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done

.PHONY: all mkfs clean
