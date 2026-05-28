#include "interrupt.h"
#include "PIC.h"
#include "io.h"
#include "debug.h"

__attribute__((aligned(0x10))) //this might be important :shrug:
idt_entry_t idt[256];

idtr_t idtr;

extern uint64_t isr_stub_table[];

isr_t routineHandlers[256];

void createIDT(){

   //this is the struct going into the register
   idtr.base = (uintptr_t)&idt[0]; 
   idtr.limit = (uint16_t)sizeof(idt_entry_t) * 40;

   //set the first 32 isr's to be errors
   for (uint8_t vector = 0; vector < 32; vector++) {

      idt_set_descriptor(vector, (void*)isr_stub_table[vector], 0x8E);
   
   }

   PIC_remap(0x20, 0x28);

   for (uint8_t vector = 32; vector < 40; vector++){

      idt_set_descriptor(vector, (void*)isr_stub_table[vector], 0x8E);

   }

   __asm__ volatile ("lidt %0" : : "m"(idtr)); //load the idt
   __asm__ volatile ("sti"); //just to be sure
   inb(0x60); // not sure at all what this is

}

void registerInterupt(uint8_t vector, isr_t handler){

   routineHandlers[vector] = handler;

}

void exceptionHandler(registers_t r){
   
   __asm__ volatile ("xchg %bx, %bx");
   kprintf("int: we recieved an interupt number %i (%h)\n", r.int_no, r.int_no);

   if (r.int_no == 14 || r.int_no == 13) {
   
      uint64_t cr2_val;
      __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2_val));
      kprintf("int: page fault address: %h\n", cr2_val);
      if(r.int_no == 14) kprintf("page fault occured, rip: %h\n", r.rip);
      if(r.int_no == 13) kprintf("GP fault\n");

   }

   if(r.int_no == 6) kprintf("invalid opcode\n");

   uint64_t errorCode;

   uint64_t int_ch = r.int_no + '0';
   //putch(int_ch, 2, 0);           //set 3e character to the error code (single digit only)
   __asm__ volatile ("pop %rax");
   __asm__ volatile ("mov %%rax, %0" : "=r"(errorCode));
   kprintf("int: error code: %h\n", errorCode);
   __asm__ volatile ("cli; hlt"); //halt when exception comes in

}

uint64_t* saved_rsp = 0;
void irq_handler(registers_t r){
   //kprintf("irq %i got triggerd from %h\n", r.int_no, r.rip);

   if(r.int_no > 0){

      //resolve and call handler
      isr_t handler = routineHandlers[r.int_no];
      handler(r);

   } 

   //bochsBreak();

  // kprintf("sending EIO\n");

   PIC_sendEOI(1);
   
   /*kprintf("sent EIO\n");

   __asm__ volatile ("mov %rsp, saved_rsp(%rip)");

   for(uint8_t i = 32; i > 0; i--){
      
      kprintf("%h: %h\n", saved_rsp-(8*i), *(saved_rsp-(8*i)));

   }
   kprintf("rsp: %h\n", saved_rsp);
   for(uint8_t i = 0; i < 32; i++){
      
      kprintf("%h: %h\n", saved_rsp+(8*i), *(saved_rsp+(8*i)));

   }*/

   //bochsBreak();

   return;

}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags){

   idt_entry_t* descriptor = &idt[vector];

   descriptor->isr_low     = (uint64_t)isr & 0xFFFF;
   descriptor->kernel_cs   = 0x08; //hardcoded because kernel cs is always 0x08 and we always want 0x08
   descriptor->ist         = 0;
   descriptor->attributes  = flags; 
   descriptor->isr_mid     = ((uint64_t)isr >> 16) & 0xFFFF;
   descriptor->isr_high    = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
   descriptor->reserved    = 0;

}
