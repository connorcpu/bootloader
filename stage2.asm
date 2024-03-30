[org 0x8000]
jmp do_shit

%include "gdt.asm"
%include "utils.asm"

do_shit:
   
   ;print that stage 2 is operational
   mov bx, stage2Msg
   call print

   ;enable and print a20
   call enableA20
   mov bx, a20Msg
   call print

   ;load gdt
   call loadGDT
   mov bx, gdtMsg
   call print

   ;this sets cr0 to state the system is in 32 bit mode
   mov eax, cr0
   or eax, 1
   mov cr0, eax

   ;jump to protected mode
   jmp codeseg:startProtecMode

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

   ;whole of gdt tabel is stolen, but should work
   ; it has 2 32-bit segments in it, one for code and one for data so it doesnt support switching back to 16 bits for bios interupts
   cli
   lgdt [gdt_descriptor]
   sti
   ret

;enable protected mode 
[bits 32]
startProtecMode:
   ;reload section registers so that the gdt takes effect
   mov ax, dataseg
   mov ds, ax
   mov es, ax
   mov ss, ax
   mov fs, ax
   mov gs, ax
   
   mov bx, msg
   call printVGA

   ;safety
;   cli
;   hlt

   ;TODO
   ;ignore checking cpuid and long mode
   ;"setup identity paging"
   ;edit gdt so it has 64 bit sectors
   call edit_gdt

   ;jump to long mode
   jmp codeseg:startLongMode

   cli
   hlt

msg: db "Welcome to connOS", 0x0

printVGA:

   xor ecx, ecx

   .loop:

      mov al, [bx]
      cmp al, 0x0
      je .exit
      mov byte [0xB8000 + 2 * ((13 * 80) + ecx)], al
      inc bx 
      inc ecx
      jmp .loop

   .exit:
      ret

   jmp codeseg:startLongMode

;start long mode
[bits 64]
startLongMode:
   mov ax, dataseg
   mov ds, ax
   mov es, ax
   mov ss, ax
   mov fs, ax
   mov gs, ax

   ;mov rax, 0xABCDEF12345689
   cli 
   hlt
   mov ebx, msg64
   call print64

   cli 
   hlt
   jmp $

msg64: db "we have reached 64-bits, congratulations!", 0x0


print64:

   xor rcx, rcx

   .loop:

      mov ax, [ebx]
      cmp ax, 0x0
      je .exit
      mov byte [0xB8000 + 2 * ((14 * 80) + rcx)], al
      inc ebx 
      inc rcx
      jmp .loop

   .exit:
      ret
