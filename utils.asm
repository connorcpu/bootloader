print:
   mov ah, 0x0E

   .loop:

      mov al, [bx]
      cmp al, 0x0 
      je .exit
      int 10h
      inc bx
      jmp .loop

   .exit:
      ret
