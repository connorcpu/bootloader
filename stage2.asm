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

   ;set first bit of cr0 which means protected mode
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
  ; sti
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
   
   mov esi, 0x0e
   mov bx, barMsg
   call printVGA
   mov bx, msg
   call printVGA
   mov bx, a32bitMsg
   call printVGA

   ;safety
;   cli
;   hlt

   ;TODO
   ;ignore checking cpuid and long mode
   call checkCpuid
   ;"setup identity paging"
   call setupPaging

   ;edit gdt so it has 64 bit sectors
   call edit_gdt

   ;jump to long mode
   jmp codeseg:startLongMode

   cli
   hlt

barMsg: db "---------------------------------- 32-bits ------------------------------------", 0x0
msg: db "Welcome to connOS", 0x0
a32bitMsg: db "you are operating in 32-bit protected mode", 0x0

printVGA:

   xor ecx, ecx
   xor eax, eax 
   xor edi, edi

   .loop:

      mov eax, esi ;esi wil be 13 or whatever line we want
      mov edi, 0x50 ;edi will be 80
      mul edi
      add eax, ecx ;add ecx which is where the x value goes
      mov edi, eax
      mov al, [bx]
      cmp al, 0x0
      je .exit
      mov byte [0xB8000 + (2 * edi)], al ; mov al into the desired memory address
     ;mov byte [0xB8000 + 2 * ((13 * 80) + ecx)], al
     ;mov byte [0xB8000 + 2 * ((y * 80) + x){OPT: + 1}, al ;add opt +1 if you wanna set the color
      inc bx 
      inc ecx
      jmp .loop

   .exit:
      inc esi
      ret


;this function assumes support for the cpuid fuction which historicly not all pc's do but fuck em
;in reality this fuction checks support for long mode
checkCpuid:
   
   ;this should make it so we can read long mode support from edx's 29'th bit
   mov eax, 80000001h
   cpuid
   ;basicly compare edx and 0x01 but shifted 29 bits left, test runs an AND on those 2 bits, the zero flag gets set as result
   test edx, 1 << 29

   ;set to 1 if the and resulted in 0 so testing 1 and the 29'th bit should make 0, setting the zf (zero flag) flag we are jumping on
   ;jump if failure
   jz shit
   
   ;we havent jumped so 64 bit is available
   ret

shitMsg: db "you fucked up", 0x0d, 0x0a, 0x0

shit:
   mov bx, shitMsg
   call printVGA

   cli 
   hlt
   jmp $

paging_table_entry equ 0x1000
setupPaging:
   
   ;create a paging table
   
   ;fill cr3 with 0x1000, paging table will be at 0x1000?
   mov edi, paging_table_entry
   mov cr3, edi

   ;no clue what this does
   mov dword [edi], 0x2003
   add edi, 0x1000
   mov dword [edi], 0x3003
   add edi, 0x1000 
   mov dword [edi], 0x4003
   add edi, 0x1000

   ;the loop instruction decrements ecx and jumps if not zero at the end, so 512 iterations
   mov ebx, 0x00000003
   mov ecx, 512

   ;add 0xn000 every 8 bytes starting at 0x4000, incrementing n every loop
   ;this does something and is important
   .set_entry:
      mov dword [edi], ebx
      add ebx, 0x1000 
      add edi, 8
      loop .set_entry

   ;we skip disabing paging because we were in gdt mode 
   ;asm garbage for switch bit num 5 in cr0
   mov edx, cr4 
   or edx, (1 << 5)
   mov cr4, ebx

   ;asm garbage for switch bit num 8 in the model spcific register 
   ;no clue what ecx does: consult wikipedia
   mov ecx, 0xC0000080
   rdmsr
   or eax, 1 << 8
   wrmsr

   ;set paging bit in cr0
   ;cr0 also has our protected mode bit so we can do this or with (1 << 31) | (1 <<0) wich can be done without first extracing cr0 into eax
   mov eax, cr0 
   or eax, 1 << 31
   mov cr0, eax

   ret

;start long mode
[bits 64]
startLongMode:
   mov ax, dataseg
   mov ds, ax
   mov es, ax
   mov ss, ax
   mov fs, ax
   mov gs, ax

   mov edi, 0xb8000 
   mov rax, 0x0f200f200f20
   mov ecx, 500
   rep stosq

   mov rax, 0xABCDEF12345689
   mov byte [0xB8000], 'C'
   mov byte [0xB8002], 'O'
   ;cli 
   ;hlt
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
