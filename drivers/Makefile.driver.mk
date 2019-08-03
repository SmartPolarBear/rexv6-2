ifndef TOP_SRCDIR
    TOP_SRCDIR = ..
endif
include $(TOP_SRCDIR)/Makefile.mk

OUTDIR=$(BUILDDIR)/drivers

_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))

all:$(_OBJS)

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	@echo "[CC]" $< "->" $@
	@($(CC) $< $(CFLAGS) -c -o $@)

$(OUTDIR)/%.o: %.S | $(OUTDIR)
	@echo "[CC]" $< "->" $@
	@($(CC) $< $(CFLAGS) -c -o $@)

$(OUTDIR):
	@echo "MKDIR" $@
	@mkdir -p $@

clean:
	@for o in $(_OBJS); do echo "REMOVE" $$o; rm -f $$o; done
	@rm -f $(OUTDIR)/*.d
	@rm -f $(OUTDIR)/*.asm
	@rm -f $(OUTDIR)/*.out

.PHONY: all clean