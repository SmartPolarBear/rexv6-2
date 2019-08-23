TOP_SRCDIR = ../..
KERN_SRCDIR= ..
include $(TOP_SRCDIR)/Makefile.mk

OUTDIR=$(BUILDDIR)/tests/vector
TARGETNAME=vector
TARGET=$(OUTDIR)/$(TARGETNAME)

OBJS = Vector.o
_OBJS = $(addprefix $(OUTDIR)/,$(OBJS))

all: $(TARGET)

run: all
	@echo "Add excutable properties"
	@chmod +x $(TARGET)
	@echo "Run the target " $(TARGETNAME)
	@./$(TARGET)

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

.PHONY: all clean run

