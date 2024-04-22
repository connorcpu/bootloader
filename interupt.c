#include "interupt.h"
#include "io.h"
#include <stdint.h>

//the idt table
__attribute__((aligned(0x10))) //this makes it go vroom
idt_entry_t idt[256]; //256 idt entries

//the idtr thing
idtr_t idtr;
extern void* isr_stub_table[];

uint64_t routineHandlers[255];

void createTable(){

   idtr.base = (uintptr_t)&idt[0];
   idtr.limit = (uint16_t)sizeof(idt_entry_t) * 255;

   for (uint8_t vector = 0; vector < 32; vector++) {

      idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
      //vectors[vector] = true;
   
   }

   __asm__ volatile ("lidt %0" : : "m"(idtr)); //load idt
   __asm__ volatile ("sti"); //enable interupts

   registerInterupt(33, &keyboardHandler);

}

void registerInterupt(uint8_t vector, void* handler){

   //keyboardHandler
   //idt_set_descriptor(0x21, &keyboardHandler, 0b11100001); //1110 for interupt gate, 0, 00 for highest privalage, 1 for present
   //idt_set_descriptor(0x21, &keyboardHandler, 0x8E); //1110 for interupt gate, 0, 00 for highest privalage, 1 for present
   routineHandlers[vector] = (uint64_t)handler;
   idt_set_descriptor(vector, isr_stub_table[vector], (0x0E | 0x80) | 001);

}

//__attribute__((noreturn))
void exception_handler(){

   __asm__ volatile ("cli; hlt"); //halt when exception comes in

}

//__attribute__((noreturn))
void keyboardHandler(){

   print("recieved keypress\0");

   __asm__ volatile ("iret"); //halt when exception comes in

}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags){

   idt_entry_t* descriptor = &idt[vector];

   descriptor->isr_low     = (uint64_t)isr & 0xFFFF;
   descriptor->kernel_cs   = 0x08; //maybe dont hard code??
   descriptor->ist         = 0;
   descriptor->attributes  = flags; 
   descriptor->isr_mid     = ((uint64_t)isr >> 16) & 0xFFFF;
   descriptor->isr_high    = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
   descriptor->reserved    = 0;

}
