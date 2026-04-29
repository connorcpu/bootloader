#include "GDT.h"
#include "debug.h"
#include "io.h"
#include "memory.h"

gdtEntry_t* GDT; 
//gdtEntry_t GDT[6];

void loadGDT(){

   GDT = (gdtEntry_t*)kmalloc(sizeof(gdtEntry_t) * 6);
   bochsBreak();
   //no strict order is imposed in 64-bit as opposed to 32-bit however:
   //KDS = KCS+8
   //UDS = UCS+8
   //GDT[0] = null entry
   

   //null pointer
   //GDT[0] = (gdtEntry_t)0x00000000000000000000000000000000; //just zero 16 bytes 
   for(uint8_t i = 0; i < 16; i++){
      uint8_t* tmp = (uint8_t *)GDT+i;
      *tmp = 0x00;
   }

   //kernel code
   //code ring 0
   GDT[1] = encodeGdtEntry(1, 0);

   //kernel data
   //data ring 0
   GDT[2] = encodeGdtEntry(0, 0);

   //user code
   //code ring 3
   GDT[3] = encodeGdtEntry(1, 3);

   //user data
   //data ring 3
   GDT[4] = encodeGdtEntry(0, 3);

   //TSS
   //GDT[5] = ...;
   
   gdtrPointer_t gdtrptr;
   gdtrptr.gdtAddr = (uint64_t) GDT;
   gdtrptr.size = ((sizeof(gdtEntry_t) * 6) - 1); //subtract 1 because that's the spec

   runLGDTR();


}

void runLGDTR(){


   gdtrPointer_t ptr;
   kprintf("tada: %h\n", GDT);
   ptr.size = (sizeof(gdtEntry_t) * 6) - 1;
   ptr.gdtAddr = (uint64_t)GDT;
   bochsBreak();

   __asm__ volatile("cli");
   __asm__ volatile("lgdt %0" : : "m" (ptr)); 
   __asm__ volatile("sti");
   
}

//type 1 for code, type 0 for data
gdtEntry_t encodeGdtEntry(uint8_t type, uint8_t ring){
   
   //limit and base are ignored in 64-bits so don't bother
   gdtEntry_t target = *((gdtEntry_t *)kmalloc(sizeof(gdtEntry_t)));
   //gdtEntry_t target;
   /*target.access = source.accessByte; 
   target.limitFlagsBase |= (source.flags << 4);
   target.base = 0x00*/

   target.limitLow = 0xFFFF;
   target.limitHigh = 0xF;
   target.baseLow = 0x000000;
   target.baseHigh = 0x00;
   target.present = 1; //must be set to 1 for valid segment
   target.ring = ring;
   target.codeOrData = 1; //yes target is code or data
   target.code = type;
   target.conforming = 0; // not quite sure yet
   target.read_write = 1; // for code this means you can read, for data it means you can write
   target.accessed = 0; //we haven't touched it yet and we are not doing read-only gdt memory (can cause page fault by CPU trying to set it to 1)
   target.granularity = 1; //1 means page, 0 means byte, the recomendation is page
   target.longT = type;
   target.big = ~type; //also know as the db size flag, should be set to 0 if the "long mode code segment" bit is set, so for data segments it should be set

   return target;


}
