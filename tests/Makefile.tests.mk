TARGET=$(OUTDIR)/$(TARGETNAME)
LINKER =  -L/usr/local/lib -Wl,-Bdynamic -lboost_unit_test_framework 
MACROS = -D'BOOST_TEST_DYN_LINK' -D'BOOST_TEST_MODULE=$(TARGETNAME)'
all: $(TARGET)

run: all
	@chmod +x $(TARGET)
	@./$(TARGET) --log_level=test_suite

$(TARGET): $(_OBJS)
	@echo "[GCC LINKER]" $^ "->" $@
	@$(HOST_CXX) $(_OBJS) $(LINKER) -o $@


$(OUTDIR)/%.o: %.cc | $(OUTDIR)
	@echo "[HOST CXX]" $< "->" $@
	@($(HOST_CXX) $< $(HOST_CXXFLAGS) $(MACROS) -c -o $@)

clean: 
	@for o in $(_OBJS); do echo "REMOVE" $$o; rm -f $$o; done
	@for o in $(TARGET); do echo "REMOVE" $$o; rm -f $$o; done


$(OUTDIR):
	@echo "MKDIR" $@
	@mkdir -p $@

.PHONY: all clean run