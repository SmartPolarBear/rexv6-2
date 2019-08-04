TOP_SRCDIR = ../..
KERN_SRCDIR= ..
include $(TOP_SRCDIR)/Makefile.mk

OUTDIR=$(BUILDDIR)/tools/mkfs
TARGETNAME=mkfs
TARGET=$(OUTDIR)/$(TARGETNAME)

OBJS = mkfs.o
_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))

all: $(TARGET)

$(TARGET): $(_OBJS)
	@echo "[GCC LINKER]" $< "->" $@
	@gcc $(_OBJS) -o $@

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	@echo "[HOST CC]" $< "->" $@
	@($(HOST_CC) $< $(HOST_CFLAGS) -c -o $@)

clean: 
	@for o in $(_OBJS); do echo "REMOVE" $$o; rm -f $$o; done
	@for o in $(TARGET); do echo "REMOVE" $$o; rm -f $$o; done

$(OUTDIR):
	@echo "MKDIR" $@
	@mkdir -p $@

.PHONY: all clean

