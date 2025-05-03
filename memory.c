#include "memory.h"
#include "io.h"
#include <stdint.h>

uint64_t* PML4;
uint64_t* pdpt;
uint64_t* pageDir;
uint64_t* pageTable;

uint64_t currentTable;
uint64_t freeMemAddr;
extern E820MemBlock memMap[256];
//E820MemBlock memMap[10];

void pagingInit(){

   uint8_t i = 0;
   while (memMap[i].length != 0) {
   
/*      kprintf("e820: found a region of type %d\n", memMap[i].type);
      kprintf("base: %d\n", memMap[i].base);
      kprintf("length: %d\n", memMap[i].length);
*/
      i++;
   }


   PML4 = (uint64_t*) PML4ADDR; 
   pdpt = (uint64_t*) 0x2000;
   pageDir = (uint64_t*) 0x3000;
   pageTable = (uint64_t*) 0x4000;

   //experimental
   //pageTable[1] = 

   kprintf("PML4: %d\n", PML4[0]);
   kprintf("pdpt: %d\n", pdpt[0]);
   kprintf("PD: %d\n", pageDir[1]);
   kprintf("PT: %d\n", pageTable[0]);

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

uint8_t mapPage(void* physAddr, void* virtAddr, uint16_t flags){

   uint32_t pdinx = (uint32_t)virtAddr >> 22;
   uint32_t ptinx = (uint32_t)virtAddr >> 12 & 0x03FF;

   if(pdpt[pdinx] != 0x03){
     //check for present bit instead of != 0x03, if not present, make it 
   }

   uint32_t* pd = (uint32_t *)0xFFFFF000;

   uint32_t* pt = ((uint32_t *)0xFFC00000) + (0x400 * pdinx);

   pt[ptinx] = ((uint32_t)physAddr) | (flags & 0xFFF) | 0x01; //present

}
