ARCH:=$(shell uname -p)

ifeq ($(ARCH), ppc64)
CC=gcc
else
CC=powerpc64-linux-gnu-gcc
endif

CPPFLAGS=-U_FORTIFY_SOURCE
CFLAGS=-g -O2 -std=c99 -fno-stack-protector
LDFLAGS=-Wl,-Ttext-segment,0x0700000000000000

PROGRAMS=hello args

.SUFFIXES:
.SECONDARY:

all: $(PROGRAMS)

%.o: %.c
	$(CC) -c -o $@ $^ $(CPPFLAGS) $(CFLAGS)

%: %.o libc.a
	$(CC) -static -nostdlib -o $@ $^ $(LDFLAGS)

libc.a: libc.o printf.o syscalls.o
	ar crs $@ $^

clean:
	rm -f *.o libc.a $(PROGRAMS)
