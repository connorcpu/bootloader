#include "memory.h"
#include "io.h"
#include <stdint.h>

uint32_t* PML4;
uint32_t* pdpt;
uint32_t* pageDir;
uint32_t* pageTable;

uint32_t currentTable;
uint32_t freeMemAddr;
//extern E820MemBlock memMap[256];
E820MemBlock memMap[10];

void pagingInit(){

   uint8_t i = 0;
   while (memMap[i].length != 0) {
   
      kprintf("e820: found a region of type %d\n", memMap[i].type);
      //kprintf("base: %d\n", memMap[i].base);
      //kprintf("length: %d\n", memMap[i].length);

      i++;
   }


   PML4 = (uint32_t*) PML4ADDR; 
   pdpt = (uint32_t*) PML4ADDR + 0x1000;
   pageDir = (uint32_t*) pdpt + 0x1000;
   pageTable = (uint32_t*) pageDir + 0x1000;

   currentTable = pageTable[0];

//   kprintf("PML4: %d\n", PML4);
//   kprintf("pdpt: %d\n", pdpt);
//   kprintf("PD: %d\n", pageDir);
//   kprintf("PT: %d\n", pageTable);
//   kprintf("current table: %d\n", &currentTable);

   freeMemAddr = 0x10000;

}

uint32_t kmalloc(uint32_t size, uint32_t *physAddr){
   //since physaddr is an empty pointer to where the allocated memory is located we just set it to where we have space
   if(physAddr) *physAddr = freeMemAddr;

   uint32_t ret = freeMemAddr; //dunno why we return the address twice but okay
   freeMemAddr += size; //move pointer to new free space
   kprintf("mem: %d\n", ret);
   return ret;

}
