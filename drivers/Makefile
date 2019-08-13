TOP_SRCDIR = ..

SUBDIRS = apic console cpu ide kbd pic uart timer

all: $(SUBDIRS)

.PHONY: all $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MFLAGS) all

clean: 
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done

.PHONY: clean