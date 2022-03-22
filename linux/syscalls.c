#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#pragma GCC optimize ("O3")

#define TARGET_PASE

#ifdef TARGET_PASE
#define SYSCALL_REG 2
#define SC_WRITE 382
#define SC_EXIT 118
#else
#define SYSCALL_REG 0
#define SC_WRITE 4
#define SC_EXIT 234
#endif

__attribute__((noinline))
ssize_t write(int fd, const void *buf, size_t size) {
#ifdef TARGET_PASE
    __asm__(
        "std 2,40(1)\n\t"
        "li  2,382\n\t"
        "sc\n\t"
        "ld  2,40(1)\n\t"
       );
#else
    __asm__(
        "std 2,40(1)\n\t"
        "li  0,4\n\t"
        "sc\n\t"
        "ld  2,40(1)\n\t"
       );
#endif
}

__attribute__((noinline))
void exit(int status) {
#ifdef TARGET_PASE
    __asm__(
        "std 2,40(1)\n\t"
        "li  2,118\n\t"
        "sc\n\t"
        "ld  2,40(1)\n\t"
       );
#else
    __asm__(
       "std 2,40(1)\n\t"
        "li  0,234\n\t"
        "sc\n\t"
        "ld  2,40(1)\n\t"
       );
#endif
    __builtin_unreachable();
}
