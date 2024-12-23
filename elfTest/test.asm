push rdi 
push rsi 
push rbp 
push rsp 
push rbx 
push rdx 
push rcx 
push rax 

mov ax, ds 
push rax 

;make sure we are running in kernel level data segment (gdt)
mov ax, 0x10 
mov ds, ax 
mov es, ax 
mov fs, ax 
mov gs, ax

mov byte [0xB8000], 'C'
mov byte [0xB8002], 'O'

;restore saved ax
pop rax 
mov ds, ax 
mov es, ax 
mov fs, ax 
mov gs, ax 

pop rax 
pop rcx 
pop rdx 
pop rbx 
pop rsp 
pop rbp 
pop rsi 
pop rdi

sti 
ret
