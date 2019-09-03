TOP_SRCDIR = ../..
BIN_SRCDIR= ..

ULIB = $(ULIBCDIR)/ulib.o \
	$(ULIBDIR)/usys.o \
	$(ULIBCDIR)/umalloc.o \
	$(ULIBCDIR)/printf.o \

OUTDIR=$(BUILDDIR)/bin/stressfs
OBJS = stressfs.o
TARGETNAME = stressfs
TARGET = $(OUTDIR)/$(TARGETNAME)
include $(BIN_SRCDIR)/Makefile.bin.mk
