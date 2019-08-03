TOP_SRCDIR = ..

SUBDIRS = init locking mm sched syscall trap

all: $(SUBDIRS)
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir all;	done

clean: 
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done