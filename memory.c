#include "memory.h"
#include "io.h"
#include <stdint.h>

uint32_t* PML4;
uint32_t* pdpt;
uint32_t* pageDir;
uint32_t* pageTable;

uint32_t currentTable;
uint32_t freeMemAddr;

void pagingInit(){

   PML4 = (uint32_t*) PML4ADDR; 
   pdpt = (uint32_t*) PML4ADDR + 0x1000;
   pageDir = (uint32_t*) pdpt + 0x1000;
   pageTable = (uint32_t*) pageDir + 0x1000;

   currentTable = pageTable[0];

   kprintf("PML4: %d\n", PML4);
   kprintf("pdpt: %d\n", pdpt);
   kprintf("PD: %d\n", pageDir);
   kprintf("PT: %d\n", pageTable);
   kprintf("current table: %d\n", currentTable);

   freeMemAddr = 0x10000;

}

//USAGE: specialType_t* varOfSpecailType = (specialType_t*)kmalloc(size, 0);
uint32_t kmalloc(uint32_t size, uint32_t *physAddr){

   //since physaddr is an empty pointer to where the allocated memory is located we just set it to where we have space
   if(physAddr) *physAddr = freeMemAddr;

   uint32_t ret = freeMemAddr; //dunno why we return the address twice but okay
   freeMemAddr += size; //move pointer to new free space
   return ret;

}
