TOP_SRCDIR = ..

SUBDIRS = mkfs
all: $(SUBDIRS)

.PHONY: all $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MFLAGS) all

clean: 
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done
	rm -rf pheader/bin
	rm -rf pheader/obj

.PHONY: all clean