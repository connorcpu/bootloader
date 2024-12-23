#include "ELF.h"
#include "io.h"
#include <stdint.h>

elf64Header_t* header;

uint8_t executeElf(fileHeader_t* file){

   header = (elf64Header_t *) file;

   if(header->bits != 2){

      kprintf("unsupported bit count\n");
      return 1;

   }else if (header->instructionSet != 0x3E) {

      kprintf("unsupported architectur\n");
      return 2;
   
   }else if (header->type != 2) {
      kprintf("file is not executable\n");
   
   }

   kprintf("progam header offset: %d\n", header->programHeaderOffset);
   kprintf("program enty offset: %d\n", header->programEntryOffset);
   kprintf("section header table offset: %d\n", header->sectionHeaderOffset);

   return 0;

}

typedef int(*execer)();

void executeRaw(fileHeader_t* file){

   int (*executablePtr)() = (execer)file;
   (*executablePtr)();

   return;

}
