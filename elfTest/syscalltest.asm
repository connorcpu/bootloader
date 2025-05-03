mov rax, 1
mov rdi, 1
mov rsi, message
mov rdx, message_length

;syscall
int 0x02

message : db "Hello world", 10
message_length equ $-message
