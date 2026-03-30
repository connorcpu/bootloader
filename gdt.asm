gdt_nulldesc:
    dd 0, 0

;both segments are very similair, only 1 bit is diffrent
gdt_codedesc:
    dw 0xFFFF ;limit
    dw 0x0000 ;base
    db 0x00  ;more base
    db 10011010b ;this line contains under more that we want 32 bit; access byte
    db 11001111b ;limit + flags..... first 4 bits are flags?
    db 0x00 ;more base

gdt_datadesc:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    gdt_size:
        dw gdt_end - gdt_nulldesc - 1
        dd gdt_nulldesc

codeseg equ gdt_codedesc - gdt_nulldesc
dataseg equ gdt_datadesc - gdt_nulldesc

[bits 32]
;edit gdt to make the sectors 64 bit
edit_gdt:
    mov [gdt_codedesc + 6], byte 10101111b
    mov [gdt_datadesc + 6], byte 10101111b
    ret

[bits 16] ;without this line no 32 bit code execution
