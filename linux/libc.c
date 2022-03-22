#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

size_t strlen(const char *s) {
    size_t size = 0;
    while (*s) {
        size++;
        s++;
    }
    return size;
}

int putchar(int c) {
    char x = (char) c;
    if (write(1, &x, 1) < 0) {
        return EOF;
    }
    return (int) c;
}

int puts(const char *s) {
    size_t len = strlen(s);
    ssize_t written = write(1, s, len);
    if (written < 0) return EOF;
    int rc = putchar('\n');
    if (rc < 0) return EOF;
    return written + 1;
}

int main(int argc, char** argv, char** envp);

#pragma GCC optimize ("O3")

char** environ;

__attribute__((noreturn))
void _start(void) {

#ifdef __powerpc64__
#if 0
0x100004f0 <.main+176>   lwz     r0,0(r4)
0x100004f4 <._start>     mr      r9,r1
0x100004f8 <._start+4>   rldicr  r1,r1,0,59
0x100004fc <._start+8>   li      r0,0
0x10000500 <._start+12>  stdu    r1,-128(r1)
0x10000504 <._start+16>  mtlr    r0
0x10000508 <._start+20>  std     r0,0(r1)
0x1000050c <._start+24>  ld      r8,-32752(r2)
0x10000510 <._start+28>  b       0x10000414 <0000003a.plt_call.__libc_start_main@@GLIBC_2.3>
#endif
    __asm__(
#if 0
        "mr 9,1\n\t"
//        "rldicr 1,1,0,59\n\t"
//        "stdu 1,-128(1)\n\t"
//        "li 0,0\n\t"
//        "mtlr 0\n\t"
//        "std 0,0(1)\n\t"
        "ld 3,0(9)\n\t" // load argc
        "addi 4,9,8\n\t" // load argv
        "addi 5,3,1\n\t"
        "rldicr 5,5,3,60\n\t" // load envp offset
        "add    5,5,4\n\t"    // load envp
#endif
	    "addis 9,2,environ@toc@ha\n\t"
	    "addi 9,9,environ@toc@l\n\t"
        "std    5,0(9)\n\t"
        "li 9,0\n\t"
        "bl main\n\t"
        "nop\n\t"
        "bl exit\n\t"
        "nop\n\t"
       );
    __builtin_unreachable();
#else
    exit(0);
#endif
}

