TOP_SRCDIR = ..
include $(TOP_SRCDIR)/rexv6.mk

SUBDIRS = libc ulib uprogs

.PHONY: all install clean

all:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir all;	done

install:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir install; done

clean:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done
