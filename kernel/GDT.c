#include "GDT.h"
#include "debug.h"
#include "io.h"
#include "memory.h"
#include "tss.h"

gdtEntry_t* GDT; 
//gdtEntry_t GDT[6];
tss_t* tss;

void loadGDT(){

   GDT = (gdtEntry_t*)kmalloc(sizeof(gdtEntry_t) * 7);
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

   //user code and date need to be swapped because of 64-bit sysret

   //TSS
   //flag byte == 0x0
   gdtEntry_t* tssEntry = (gdtEntry_t*)kmalloc(sizeof(gdtEntry_t) * 2);
   tssEntry->granularity = 0;
   tssEntry->big = 0;
   tssEntry->longT = 0;
   
   //access byte: 0x89: 0b10001001
   //0b1000 means present; ring zero (2 bits); data segment
   //1001 means type 0x9, 64-bits tss segment
   tssEntry->present = 1;
   tssEntry->ring = 0;
   tssEntry->codeOrData = 0; //only 0 for TSS or LDT
   tssEntry->code = 1;  //code, conforming, read_write and accessed should be a 4-bit type field, 0x9 = 64-bit tss (available)
   tssEntry->conforming = 0; //always zero on TSS
   tssEntry->read_write = 0; //can be seen as 1=busy, 0=avaibable
   tssEntry->accessed = 1; //should be 1=tss, 0=LDT as long as codeOrDate = 0 because that means system segment
   
   tss = (tss_t*)kmalloc(sizeof(tss_t));
   kprintf("tss: %h\n", tss);
   uint64_t limit = sizeof(tss_t) - 1;
   tssEntry->baseLow = (uint32_t)tss & 0xFFFFFF;
   tssEntry->baseHigh = ((uint64_t)tss >> 24) & 0xFF; //shouldn't even need the & 0xFF;
   tssEntry->limitLow = limit;
   tssEntry->limitHigh = (limit >> 16) & 0xF;

   kprintf("size: %i\n", sizeof(tss_t));
   
   gdtSysExtension_t* tssExtension = (gdtSysExtension_t*)(tssEntry + 1); //whaaaaat, +8 does +8 gdtEntry sizes???
   tssExtension->baseMegaHigh = ((uint64_t)tss >> 32) & 0xFFFFFFFF;
   tssExtension->reserved = 0x0;

   GDT[6] = *((gdtEntry_t*)(tssExtension));
   GDT[5] = *tssEntry;
   
   //memcpy(&GDT[5], tssEntry, 16);
   kprintf("tss access byte, should be 0x89: %h, at addr: %h\n", *(((uint8_t*)(GDT + 5)) + 5), ((uint8_t*)(GDT + 5)) + 5);
   kprintf("gdt: %h, %h\n", GDT, (5*8));

   //filling the TSS
   tss->RSP2_l = 0x10000;
   tss->RSP2_h = 0x00000000;
   
   
   gdtrPointer_t gdtrptr;
   gdtrptr.gdtAddr = (uint64_t) GDT;
   gdtrptr.size = ((sizeof(gdtEntry_t) * 7) - 1); //subtract 1 because that's the spec

   runLGDTR();


}

void runLGDTR(){


   gdtrPointer_t ptr;
   ptr.size = (sizeof(gdtEntry_t) * 7) - 1;
   ptr.gdtAddr = (uint64_t)GDT;

   __asm__ volatile("cli");
   __asm__ volatile("lgdt %0" : : "m" (ptr)); 
   __asm__ volatile("mov $0x28, %ax");
   //__asm__ volatile("xchg %bx, %bx");
   __asm__ volatile("ltr %ax");
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
