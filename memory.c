#include <stdint.h>
#include "memory.h"
#include "io.h"

uint32_t* PML4;
uint32_t* pdpt;
uint32_t* pageDir;
uint32_t* pageTable;

uint32_t currentTable;

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

}
