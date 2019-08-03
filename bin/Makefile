TOP_SRCDIR = ..

SUBDIRS = cat echo forktest grep init kill ln ls mkdir rm sh stressfs usertests wc zombie 

all: $(SUBDIRS)

.PHONY: all $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MFLAGS) all

clean: 
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done

.PHONY: all clean