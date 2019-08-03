TOP_SRCDIR = ..

SUBDIRS = apic console cpu ide kbd pic uart

all: $(SUBDIRS)
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir all;	done

clean: 
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done