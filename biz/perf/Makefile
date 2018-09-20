
HVERSION=$(shell uname -a)

ifneq (,$(findstring x86_64,$(HVERSION)))
	SRCDIR = x86_64
else
	SRCDIR = i386
endif

all:
	@(cd $(SRCDIR) && make)
	cp $(SRCDIR)/libperf.a .
	#cp $(SRCDIR)/perf.lib .

clean:
	@(cd $(SRCDIR) && make clean)
	rm -f libperf.a perf.lib
