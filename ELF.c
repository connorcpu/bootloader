#include "ELF.h"
#include "io.h"
#include <stdint.h>
#include "utils.h"
#include "memory.h"

elf64Header_t* header;

uint8_t executeElf(fileHeader_t* file){

   header = (elf64Header_t *) file;

   kprintf("magic num: %d\n", header->magic);
   kprintf("header location: %d\n", file);

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
   
   }

   kprintf("no issues found in elf\n");

   programHeaderEntry_t* programHeaderTable = (void*)((uint64_t)header->programHeaderOffset + (uint64_t)header);

   kprintf("programHeaderTableaddr: %d\n", header->programHeaderOffset);

   uint64_t* pml4 = alloc_page();

   for(uint8_t i = 0; i < header->programHeaderEntryCount; i++){

      if (programHeaderTable[i].type != 1) {
         continue;
      }

      kprintf("program header type: %d\n", programHeaderTable[i].type);
      kprintf("program header offset: %d\n", programHeaderTable[i].p_offset);
      kprintf("program header virtAddr: %d\n", programHeaderTable[i].p_vaddr);
      kprintf("program header physAddr: %d\n", programHeaderTable[i].p_paddr);
      kprintf("program header fileSize: %d\n", programHeaderTable[i].p_filesz);
      kprintf("program header memSize: %d\n", programHeaderTable[i].p_memsz);
      kprintf("program header entry: %d\n", programHeaderTable[i].p_memsz);


      //allocate space for the data to be loaded 
      //then map it to the desired virtual address is the programs pml4 table
      uint8_t* dataLoc = kmalloc(programHeaderTable[i].p_memsz);
      mappage(pml4, dataLoc, (void*)programHeaderTable[i].p_vaddr, 0x0);

      //copy the data over
      memcpy((void*)programHeaderTable[i].p_offset, dataLoc, programHeaderTable[i].p_memsz);

      //hand off execution

   }


   kprintf("progam header offset: %d\n", header->programHeaderOffset);
   kprintf("program enty offset: %d\n", header->programEntryOffset);
   kprintf("section header table offset: %d\n", header->sectionHeaderOffset);

   return 0;

}

typedef int(*execer)();

void executeRaw(fileHeader_t* file){

//   int (*executablePtr)() = (execer)file;
  // (*executablePtr)();
   void (*program)(void) = (void (*)())file;
   program();

   return;

}
