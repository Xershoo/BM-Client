
OBJS = md5.o sha1.o rc4.o mppc256.o base64.o crc32.o aes.o bf.o

all: libperf.a 

.s.o:	
	$(CC) -c $<

libperf.a: $(OBJS)
	ar -ru $@ $(OBJS)
	strip --discard-all $@

clean:
	rm -f $(OBJS) libperf.a perf.lib
