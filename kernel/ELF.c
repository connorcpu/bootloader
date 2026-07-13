#include "ELF.h"
#include "io.h"
#include <stdint.h>
#include "memory.h"
#include "debug.h"
#include "tss.h"

elf64Header_t* header;

extern tss_t* tss;

uint64_t rsp_s;
uint64_t rbp_s;

uint8_t executeElf(fileHeader_t* file){

   //bochsBreak();
   header = (elf64Header_t *) file;

   kprintf("magic num: %h\n", header->magic);

   if (header->magic != 0x464C457F) {
      kprintf("elf: magic number is fucked\n");
      return 1;
   }

   if(header->bits != 2){

      kprintf("elf: unsupported bit count\n");
      return 2;

   }else if (header->instructionSet != 0x3E) {

      kprintf("elf: unsupported architectur\n");
      return 3;
   
   }else if (header->type != 2) {
      kprintf("elf: file is not executable\n");
      return 4;
   
   }

   kprintf("elf: opening ELF; no issues found in header\n");

   programHeaderEntry_t* programHeaderTable = (void*)((uint64_t)header->programHeaderOffset + (uint64_t)header);

   //kprintf("programHeaderTableaddr: %d\n", header->programHeaderOffset);

   for(uint8_t i = 0; i < header->programHeaderEntryCount; i++){

      if (programHeaderTable[i].type != 1) {
         continue;
      }

      /*kprintf("program header type: %d\n", programHeaderTable[i].type);
      kprintf("program header p offset: %h\n", programHeaderTable[i].p_offset);
      kprintf("program header p virtual addr: %h\n", programHeaderTable[i].p_vaddr);
      kprintf("program header p physical addr: %h\n", programHeaderTable[i].p_paddr);
      kprintf("program header p file size: %h\n", programHeaderTable[i].p_filesz);
      kprintf("program header p mem size: %h\n", programHeaderTable[i].p_memsz);
      */
      //load program headers that want to be loaded
      //mapPage((void*)programHeaderTable[i].p_paddr, (void*)programHeaderTable[i].p_vaddr, 0x0);

   }


   /*kprintf("progam header offset: %d\n", header->programHeaderOffset);
   kprintf("program enty offset: %h\n", header->programEntryOffset);
   kprintf("section header table offset: %h\n", header->sectionHeaderOffset);*/

   //page table still loaded in lower half........... this WILL be an issue eventually
   //loading time bitch
   for(uint8_t i = 0; i < header->programHeaderEntryCount; i++){
      

      //it's a load type
      if(programHeaderTable[i].type == 1){
         kprintf("mapping header #%i at addr: %h, with mem size %h\n", i, programHeaderTable[i].p_vaddr, programHeaderTable[i].p_memsz);

         //just identity map for now ig
         int64_t sizeLeft = programHeaderTable[i].p_memsz;


         do{
            mapPage((uint8_t*)(programHeaderTable[i].p_vaddr) + sizeLeft, (uint8_t*)(programHeaderTable[i].p_vaddr) + sizeLeft, 0x4);
      //      kprintf("mapping page %h\n", programHeaderTable[i].p_vaddr + sizeLeft);
            sizeLeft -= 0x1000;
         }while(sizeLeft >= 0x0000);
      }
      
      //not that the pages are mapped, time to copy the data
      //copy to the vaddr that has just been mapped, from where the file got loaded +  the specified offset, and copy the amount of bytes the header specifies
      memcpy((uint8_t*)programHeaderTable[i].p_vaddr, (uint8_t*)((uint64_t)file + (uint64_t)programHeaderTable[i].p_offset), programHeaderTable[i].p_filesz);

      //if there is more memory space in a program than there is file data to fill it with, it needs to be padded with zero's
      if(programHeaderTable[i].p_filesz < programHeaderTable[i].p_memsz){
         
         uint8_t* where = (uint8_t*)(programHeaderTable[i].p_vaddr + programHeaderTable[i].p_filesz);
         while(where <= programHeaderTable[i].p_memsz){
            *where = 0x00;
            where++;
         }
         

      }
      
      // that should be it for the loop

   }

   //now just jump to entry point in theory
   void (*program)(void) = (void (*)())header->programEntryOffset;
   kprintf("elf: jumping to program at %h\n", program);

   mapPage((uint8_t*)0x10000, (uint8_t*)0x10000, 0x4); //should create a user page
   //store stack pointers in the TSS
   rsp_s = 0x0;
   rbp_s = 0x0;
   __asm__ volatile("mov %%rsp, %0" : "=r"(rsp_s));
   __asm__ volatile("mov %%rbp, %0" : "=r"(rbp_s));
   tss->RSP0_l = rsp_s;
   tss->RSP0_h = (rsp_s >> 32);
   kprintf("saving %h\n", rbp_s);

   __asm__ volatile(
         "mov $0x23, %%ax\n\t"
         "mov %%ax, %%ds\n\t"
         "mov %%ax, %%es\n\t"
         "mov %%ax, %%gs\n\t"
         "mov %%ax, %%fs\n\t"
         "mov $0x10FFF, %%rbp\n\t"


         //"mov %%rsp, %%rax\n\t"
         "xchg %%bx, %%bx\n\t"
         "pushq $0x23\n\t"
         //"pushq %%rax\n\t"
         "pushq $0x10FFF\n\t"
         "pushfq\n\t"
         "pushq $0x1b\n\t"
         "pushq %0\n\t"
         "iretq\n\t"
         :
         : "r"(program)
         : "rax", "memory"
         );

   //program();

   return 0;

}

typedef int(*execer)();

void executeRaw(fileHeader_t* file){

   kprintf("elf: executing raw\n");

//   int (*executablePtr)() = (execer)file;
  // (*executablePtr)();
   void (*program)(void) = (void (*)())file;

   kprintf("elf: jumping to: %h\n", program);
   program();

   return;

}
