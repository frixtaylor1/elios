bits 64
global memspace
%define Heapsize (1024*1024*1024/4)

section .data alloc noexec write progbits
memspace:
    dq _memspace

section .bss
_memspace:
    resd Heapsize

section .note.GNU-stack noalloc
