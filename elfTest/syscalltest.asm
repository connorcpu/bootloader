section .text
global _start
_start:
   mov rax, 1
   mov rdi, 1
   mov rsi, message
   mov rdx, message_length

   syscall
;int 0x02

section .data
message: db "Hello world", 10
message_length equ $-message
