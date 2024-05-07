#include "interupt.h"
#include "io.h"
#include <stdint.h>

//the idt table
__attribute__((aligned(0x10))) //this makes it go vroom
idt_entry_t idt[256]; //256 idt entries

//the idtr thing
idtr_t idtr;
extern uint64_t isr_stub_table[];

isr_t routineHandlers[256];

void createTable(){

   idtr.base = (uintptr_t)&idt[0];
   idtr.limit = (uint16_t)sizeof(idt_entry_t) * 40;

   for (uint8_t vector = 0; vector < 32; vector++) {

      idt_set_descriptor(vector, (void*)isr_stub_table[vector], 0x8E);
   
   }

   PIC_remap(0x20, 0x28);

   for (uint8_t vector = 32; vector < 39; vector++){

      idt_set_descriptor(vector, (void*)isr_stub_table[vector], 0x8E);

   }

   __asm__ volatile ("lidt %0" : : "m"(idtr)); //load idt
   __asm__ volatile ("sti"); //enable interupts
   inb(0x60);

   registerInterupt(1, &keyboardHandler);

}

void registerInterupt(uint8_t vector, isr_t handler){

   routineHandlers[vector] = handler;

}

void exception_handler(registers_t r){

   print("we recieved an interupt\0");

   uint64_t int_ch = r.int_no + '0';
   putch(int_ch, 2, 0);           //set 3e character to the error code (single digit only)
   __asm__ volatile ("cli; hlt"); //halt when exception comes in

}

void irq_handler(registers_t r){

   if(r.int_no > 0){

      //resolve and call handler
      isr_t handler = routineHandlers[r.int_no];
      handler(r);

   } 

   //set 2e character to irq number
   uint64_t int_ch = r.int_no + '0';
   putch(int_ch, 1, 0); 

   PIC_sendEOI(1);

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
