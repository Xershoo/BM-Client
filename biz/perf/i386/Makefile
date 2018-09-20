
OBJS = md5.o sha1.o rc4.o mppc256.o base64.o crc32.o aes.o bf.o

all: libperf.a 
#perf.lib

.s.o:	
	$(CC) -c $<

libperf.a: $(OBJS)
	ar -ru $@ $(OBJS)
	strip --discard-all $@

perf.lib: libperf.a
	objcopy --output-target=coff-i386 --prefix-symbols="_" --change-leading-char libperf.a $@

clean:
	rm -f $(OBJS) libperf.a perf.lib
