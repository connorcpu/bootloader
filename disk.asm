;programSpace equ 0x7E00
programSpace equ 0x8000 ;make ABSOLUTELY DUBBLY SURE that this number is the same as the org at the top of stage2.asm

read_disk:
   mov ah, 0x02 ; read sectors
   mov al, 0x12; should be enough, dunno how much space we have tho
   mov ch, 0x0 
   mov cl, 0x02
   mov dh, 0x0 ;read from the very first sector?? idk man
   mov dl, [diskNum]
   mov bx, programSpace
   int 13h
   jc error 
   ret 

error:
   mov bx, errorMsg
   call print
   jmp $

diskNum:
   db 0

errorMsg: db "an error occured while reading the kernel", 0x0D, 0x0A, 0x0
   
