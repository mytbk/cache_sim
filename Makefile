CFLAGS+=-g -Wall -std=gnu99

sim: cache.o cacheop.o mem.o interface.o sim.o

.PHONY: clean
clean:
	rm -f *.o sim
