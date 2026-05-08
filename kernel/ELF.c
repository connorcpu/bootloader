#include "ELF.h"
#include "io.h"
#include <stdint.h>
#include "memory.h"
#include "debug.h"

elf64Header_t* header;

uint8_t executeElf(fileHeader_t* file){

   header = (elf64Header_t *) file;

   kprintf("magic num: %h\n", header->magic);

   if (header->magic != 0x464C457F) {
      kprintf("magic number is fucked\n");
      return 1;
   }

   if(header->bits != 2){

      kprintf("unsupported bit count\n");
      return 2;

   }else if (header->instructionSet != 0x3E) {

      kprintf("unsupported architectur\n");
      return 3;
   
   }else if (header->type != 2) {
      kprintf("file is not executable\n");
      return 4;
   
   }

   kprintf("no issues found in elf\n");

   programHeaderEntry_t* programHeaderTable = (void*)((uint64_t)header->programHeaderOffset + (uint64_t)header);

   kprintf("programHeaderTableaddr: %d\n", header->programHeaderOffset);

   for(uint8_t i = 0; i < header->programHeaderEntryCount; i++){

      if (programHeaderTable[i].type != 1) {
         continue;
      }

      kprintf("program header type: %d\n", programHeaderTable[i].type);
      kprintf("program header p offset: %h\n", programHeaderTable[i].p_offset);
      kprintf("program header p virtual addr: %h\n", programHeaderTable[i].p_vaddr);
      kprintf("program header p physical addr: %h\n", programHeaderTable[i].p_paddr);
      kprintf("program header p file size: %h\n", programHeaderTable[i].p_filesz);
      kprintf("program header p mem size: %h\n", programHeaderTable[i].p_memsz);

      //load program headers that want to be loaded
      //mapPage((void*)programHeaderTable[i].p_paddr, (void*)programHeaderTable[i].p_vaddr, 0x0);

   }


   kprintf("progam header offset: %d\n", header->programHeaderOffset);
   kprintf("program enty offset: %h\n", header->programEntryOffset);
   kprintf("section header table offset: %h\n", header->sectionHeaderOffset);

   //page table still loaded in lower half........... this WILL be an issue eventually
   //loading time bitch
   for(uint8_t i = 0; i < header->programHeaderEntryCount; i++){
      
      //it's a load type
      if(programHeaderTable[i].type == 1){

         //just identity map for now ig
         int64_t sizeLeft = programHeaderTable[i].p_memsz;

         bochsBreak();

         do{
            kprintf("mapping program header %d, at %h with sizeLeft: %h\n", i, programHeaderTable[i].p_vaddr, sizeLeft);
            mapPage((uint8_t*)(programHeaderTable[i].p_vaddr), (uint8_t*)(programHeaderTable[i].p_vaddr), 0x0);
            sizeLeft -= 0x1000;
         }while(sizeLeft > 0x0000);
      }
      
      //not that the pages are mapped, time to copy the data
      //copy to the vaddr that has just been mapped, from where the file got loaded +  the specified offset, and copy the amount of bytes the header specifies
      kprintf("going to copy mem for header number %d\n", i);
      kprintf("src: %h, file: %h, offset: %h\n", ((uint64_t)file + programHeaderTable[i].p_offset), file, programHeaderTable[i].p_offset);
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
   program();

   return 0;

}

typedef int(*execer)();

void executeRaw(fileHeader_t* file){

   kprintf("executing raw\n");

//   int (*executablePtr)() = (execer)file;
  // (*executablePtr)();
   void (*program)(void) = (void (*)())file;

   kprintf("jumping to: %h\n", program);
   program();

   return;

}
