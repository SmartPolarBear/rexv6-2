TOP_SRCDIR = ../..
KERN_SRCDIR= ..
include $(TOP_SRCDIR)/Makefile.mk

OUTDIR=$(BUILDDIR)/tools/mkfs
TARGETNAME=mkfs
TARGET=$(OUTDIR)/$(TARGETNAME)

OBJS = mkfs.o byte_order.o block.o inode.o sect.o
_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))

all: $(TARGET)

$(TARGET): $(_OBJS)
	@echo "[GCC LINKER]" $^ "->" $@
	@$(HOST_CXX) $(_OBJS) -o $@

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	@echo "[HOST CC]" $< "->" $@
	@($(HOST_CC) $< $(HOST_CFLAGS) -c -o $@)

$(OUTDIR)/%.o: %.cc | $(OUTDIR)
	@echo "[HOST CXX]" $< "->" $@
	@($(HOST_CXX) $< $(HOST_CXXFLAGS) -c -o $@)

clean: 
	@for o in $(_OBJS); do echo "REMOVE" $$o; rm -f $$o; done
	@for o in $(TARGET); do echo "REMOVE" $$o; rm -f $$o; done

$(OUTDIR):
	@echo "MKDIR" $@
	@mkdir -p $@

.PHONY: all clean

