[org 0x7c00]


mov bx, helloMsg
call print

%include "utils.asm"
helloMsg:
    db "Booting your operating system...", 0x0D, 0x0A, 0x0

times 510 - ($ - $$) db 0
dw 0xaa55
