[org 0x7c00]

cld ;clear direction flag
mov sp, 0x7c00 ;setup stack to go below where the code is loaded

xor ax, ax ;reload section registers?
mov ds, ax 
mov es, ax 
mov ss, ax 


mov [diskNum], dl

mov bx, helloMsg
call print

call read_disk

mov bx, postReadMsg
call print

jmp programSpace

%include "disk.asm"
%include "utils.asm"

helloMsg: db "Booting your operating system...", 0xD, 0xA, 0x00
postReadMsg: db "the kernel was read into memory", 0xd, 0xa, 0x00

times 510 - ($ - $$) db 0
dw 0xaa55
