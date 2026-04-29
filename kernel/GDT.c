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
   gdtCEntry_t nul;
   nul.accessByte = 0x00;
   nul.flags = 0x0;
   GDT[0] = encodeGdtEntry(nul); 

   //kernel code
   gdtCEntry_t kcs;
   kcs.accessByte = 0x9a; //10011010b: present; ring 0; code/date segment; executable; cannot be executed from other rings; read write allowed; not accessed
   kcs.flags = 0xA; //1010b, granular to pages instead of bytes; db should be clear in 64-bits; long mode flag enabled
   GDT[1] = encodeGdtEntry(kcs);

   //kernel data
   gdtCEntry_t kds;
   kds.accessByte = 0x92; //10010010b: present; ring 0; code/data segment; not executable; segment grows up; read/write enabled; not accessed
   kds.flags = 0xC; //1100b granular to bytes; 0=16-bit, 1=32-bit, set to 1 :shrug:; not long mode _code_ segment 
   GDT[2] = encodeGdtEntry(kds);

   //user code
   gdtCEntry_t ucs;
   ucs.accessByte = 0xF2;
   ucs.flags = 0xc;
   GDT[3] = encodeGdtEntry(ucs);

   //user data
   gdtCEntry_t uds;
   uds.accessByte = 0xFA;
   uds.flags = 0xA;
   GDT[4] = encodeGdtEntry(uds);

   //TSS
   //GDT[5] = ...;
   
   gdtrPointer_t gdtrptr;
   gdtrptr.gdtAddr = (uint64_t) GDT;
   gdtrptr.size = ((sizeof(gdtEntry_t) * 6) - 1); //subtract 1 because that's the spec

   runLGDTR(gdtrptr);

   kprintf("tada: %h\n", gdtrptr);

}

void runLGDTR(gdtrPointer_t ptr){

   bochsBreak();

   __asm__ volatile("lGDT %[gdtr]" : [gdtr] "=g" (ptr)); 

}

gdtEntry_t encodeGdtEntry(gdtCEntry_t source){

  //limit and base are ignored in 64-bits so don't bother
  gdtEntry_t target = *((gdtEntry_t *)kmalloc(sizeof(gdtEntry_t)));
  //gdtEntry_t target;
  target.access = source.accessByte; 
  target.limitFlagsBase |= (source.flags << 4);

}
