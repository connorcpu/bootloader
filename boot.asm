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
postReadMsg: db "the second stage was read into memory", 0xd, 0xa, 0x00

times 494 - ($ - $$) db 0
;times 2046 - ($ - $$) db 0xff

;make a single partition that starts after where the second stage is stored
db 0x80 ;drive attributes
db 0x20, 0x21, 0x00;chs addres of partition start (3 bytes)
db 0x04 ;partition type 
db 0xbe, 0x32, 0x0c; chs addres of partition end (3 bytes) H/S/C
db 0x00, 0x08, 0x00, 0x00 ;lba of partition start
db 0x00, 0x18, 0x03, 0x0 ;number of sectors in partition

dw 0xaa55
;dw 0xface
