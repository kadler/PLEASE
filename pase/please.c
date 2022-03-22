#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#ifdef VERBOSE
#define debug(fmt, ...) \
  fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  #define TOBE2(x) __builtin_bswap16(x)
  #define TOBE4(x) __builtin_bswap32(x)
  #define TOBE8(x) __builtin_bswap64(x)
#else
  #define TOBE2(x) (x)
  #define TOBE4(x) (x)
  #define TOBE8(x) (x)
#endif

extern int linux_start(int argc, char** argv, char** envp, unsigned long start_addr, unsigned long toc);

unsigned short read_short(int fd) {
    unsigned short s;
    read(fd, &s, sizeof(s));

    return TOBE2(s);
}

unsigned int read_int(int fd) {
    unsigned int i;
    read(fd, &i, sizeof(i));

    return TOBE4(i);
}

unsigned long read_long(int fd) {
    unsigned long l;
    read(fd, &l, sizeof(l));

    return TOBE8(l);
}

void usage(void);

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
    }

    const char* filename = argv[1];
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        usage();
    }

    unsigned char faddr[16];
    read(fd, faddr, sizeof(faddr));

    if (memcmp(faddr, "\x7f" "ELF", 4) != 0) {
        printf("Invalid ELF identifier!\n");
        return 1;
    }

    if (faddr[4] != 2) {
        printf("Invalid ELF class %u\n", faddr[4]);
        return 1;
    }

    if (faddr[5] != 2) {
        printf("Invalid ELF data format %u\n", faddr[5]);
        return 1;
    }

    unsigned char* cur = NULL;
    unsigned short type = read_short(fd);
    if (type != 2) {
        printf("Invalid ELF type %u\n", type);
        return 1;
    }

    unsigned short machine = read_short(fd);
    if (machine != 21) {
        printf("Invalid ELF machine %u\n", machine);
        return 1;
    }

    unsigned int version = read_int(fd);
    if (version != 1) {
        printf("Invalid ELF version %u\n", version);
        return 1;
    }

    unsigned long entry = read_long(fd);
    unsigned long pgmhdr = read_long(fd);
    unsigned long scthdr = read_long(fd);

    debug("cur: %p entry: %p, pgmhdr: %p, scthdr: %p\n", cur, entry, pgmhdr, scthdr);

    unsigned int flags = read_int(fd);

    unsigned short hdr_size = read_short(fd);

    unsigned short pgmhdr_entry_size = read_short(fd);
    unsigned short num_pgmhdr_entries = read_short(fd);
    unsigned short scthdr_entry_size = read_short(fd);
    unsigned short num_scthdr_entries = read_short(fd);
    unsigned short section_name_entry = read_short(fd);

    debug("cur: %p\n", cur);

    for (unsigned i = 0; i < num_pgmhdr_entries; ++i) {
        off_t c = lseek(fd, 0, SEEK_CUR);

        unsigned int segment_type = read_int(fd);
        unsigned int segment_flags = read_int(fd);
        unsigned long poff = read_long(fd);
        unsigned long vaddr = read_long(fd);
        unsigned long ign = read_long(fd);
        unsigned long psize = read_long(fd);
        unsigned long vsize = read_long(fd);
        unsigned long align = read_long(fd);

        lseek(fd, c + pgmhdr_entry_size, SEEK_SET);

        // Only deal with PT_LOAD entries
        if (segment_type != 1) continue;

        debug("header %d\n", i);
        debug(" type=%u\n", segment_type);
        debug(" poff=%lx vaddr=%lx\n", poff, vaddr);
        debug(" psize=%lx vsize=%lx\n", psize, vsize);
        debug(" flags=%o align=%lx\n", segment_flags, align);

        unsigned long map_vaddr = vaddr & ~4095LU;
        unsigned long map_vsize = vsize + (vaddr - map_vaddr);
        debug("mmap %p %lu %p %lu %x\n", vaddr, vsize, map_vaddr, map_vsize, segment_flags & 0x7);
        unsigned char* addr = mmap((void*) map_vaddr, map_vsize, PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED , -1, 0);
        if ((void*) addr == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }

        debug("copy %lu bytes from %lx to %p\n", psize, faddr+poff, vaddr);

        lseek(fd, poff, SEEK_SET);
        read(fd, (void*)vaddr, psize);

        unsigned long init_size = vsize - psize;
        debug("initialize %lu bytes\n", init_size);
        memset((void*)(vaddr + psize), 0, init_size);
        debug("setting permissions to %o\n", segment_flags & 0x7);
        mprotect((void*)map_vaddr, map_vsize, segment_flags & 0x7);

        lseek(fd, c + pgmhdr_entry_size, SEEK_SET);
    }

    close(fd);

    debug("entry is %p\n", entry);

    unsigned long start_addr = ((unsigned long*)entry)[0];
    unsigned long toc = ((unsigned long*)entry)[1];
    unsigned long* sp = (unsigned long*) &argv[-1];

    debug("r1 = %p\n", sp);
    debug("r2 = %p\n", toc);
    debug("jump %p\n", start_addr);

#ifdef __powerpc__
    return linux_start(argc-1, &argv[1], environ, start_addr, toc);
#else
    return 0;
#endif
}

void usage(void) {
    fprintf(stderr, "please <program> [args ...]\n");
    exit(1);
}
