[default abs]
[default rel]
[extern rsp_s]
[extern rbp_s]
[extern rsp_u]
[extern rbp_u]

%macro isr_err_stub 1
isr_stub_%+%1:
   cli
   ;push qword 0 ;almost certain i forgot this line; nope: this is auto pushed error code
   mov qword [rsp_u], rsp
   mov qword [rbp_u], rbp
   mov rbp, [rbp_s]
   ;mov rsp, rsp_s
   push qword %1
   jmp isr_common
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
   cli
   mov qword [rsp_u], rsp
   mov qword [rbp_u], rbp
   mov rbp, [rbp_s]
   ;mov rsp, rsp_s
   push qword 0
   push qword %1
   jmp isr_common
%endmacro

%macro irq_stub 2
irq_stub_%+%1:
   cli
   mov qword [rsp_u], rsp
   mov qword [rbp_u], rbp
   mov rbp, [rbp_s]
   ;mov rsp, rsp_s
   push qword 0
   push qword %2 
   ;call irq_handler
   jmp irq_common
%endmacro

isr_common:

    push r11
    push r10
    push r9
    push r8
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

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call exceptionHandler

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
    pop r8
    pop r9
    pop r10
    pop r11

    add rsp, 16; this undos the int_no and error code pushes

    mov rsp, qword [rsp_u]
    mov rbp, qword [rbp_u]

    sti
    iretq

irq_common:

   push r11
   push r10
   push r9
   push r8
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

   call irq_handler

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
   pop r8
   pop r9
   pop r10
   pop r11

   add rsp, 16
   mov rsp, qword [rsp_u]
   mov rbp, qword [rbp_u]
   sti ;needed since iretq
   ;xchg bx, bx
   iretq



[extern irq_handler]
[extern exceptionHandler]
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2 
isr_no_err_stub 3 
isr_no_err_stub 4 
isr_no_err_stub 5 
isr_no_err_stub 6 
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10 
isr_err_stub    11 
isr_err_stub    12 
isr_err_stub    13 
isr_err_stub    14 
isr_no_err_stub 15 
isr_no_err_stub 16 
isr_err_stub    17 
isr_no_err_stub 18 
isr_no_err_stub 19 
isr_no_err_stub 20 
isr_no_err_stub 21 
isr_no_err_stub 22 
isr_no_err_stub 23 
isr_no_err_stub 24 
isr_no_err_stub 25 
isr_no_err_stub 26 
isr_no_err_stub 27 
isr_no_err_stub 28 
isr_no_err_stub 29
isr_err_stub    30 
isr_no_err_stub 31
irq_stub 32, 0
irq_stub 33, 1
irq_stub 34, 2
irq_stub 35, 3
irq_stub 36, 4
irq_stub 37, 5
irq_stub 38, 6
irq_stub 39, 7

global isr_stub_table
isr_stub_table: 
%assign i 0
%rep 32
   dq isr_stub_%+i
%assign i i+1
%endrep

%assign i 32
%rep 8
   dq irq_stub_%+i
%assign i i+1
%endrep


