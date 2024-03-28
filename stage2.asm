[org 0x8000]
[bits 16]
mov bx, stage2Msg
call print

call enableA20
mov bx, a20Msg
call print

%include "gdt.asm"

call loadGDT
mov bx, gdtMsg
call print

;what does this do?
mov eax, cr0
or eax, 1
mov cr0, eax

jmp codeseg:startProtecMode

%include "utils.asm"

stage2Msg: db "stage 2 now operational", 0x0d, 0x0a, 0x0
a20Msg: db "enabled A20 gate", 0x0d, 0x0a, 0x0
gdtMsg: db "loaded gdt tabel", 0x0d, 0x0a, 0x0

;enable a20
enableA20:
   ;we should check first but nah
   in al, 0x92
   or al, 2
   out 0x92, al 
   ret

;load gdt
loadGDT:

   ;whole of gdt tabel is stolen, prob doesnt work
   cli
   lgdt [gdt_descriptor]
   sti
   ret

;enable protected mode 
[bits 32]
startProtecMode:
   ;set a bunch of things idk what they do
   mov ax, dataseg
   
   jmp codeseg:startLongMode
   
;start long mode
[bits 64]
startLongMode:
   jmp $
