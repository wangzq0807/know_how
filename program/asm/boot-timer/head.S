.code32

.text
.global _start
_start:
    xorw %ax, %ax

.org 8190
    .word 0xAA55
_end:
