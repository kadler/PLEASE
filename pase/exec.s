	.file	"exec.s"
	.toc
	.csect .text[PR]
	.align 2
	.globl linux_start
	.globl .linux_start
	.csect linux_start[DS],3
linux_start:
	.llong .linux_start, TOC[tc0], 0
	.csect .text[PR]
.linux_start:
    mtctr 6
    mr 2,7
    bctr
LT..linux_start:
	.long 0
	.byte 0,0,32,96,128,1,5,1
	.long 0
	.long LT..linux_start-.linux_start
	.short 10
	.byte "linux_start"
	.byte 31
	.align 2
_section_.text:
	.csect .data[RW],4
	.llong _section_.text
