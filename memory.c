#include "memory.h"
#include "io.h"
#include <stdint.h>

uint64_t* PML4;
//uint64_t* pdpt;
//uint64_t* pageDir;
//uint64_t* pageTable;

//uint64_t currentTable;
uint64_t freeMemAddr;
uint64_t kmallocFreeMem;
extern E820MemBlock memMap[256];
uint64_t allocEnd; //stores the highest address we can alloced without having paging problems
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
   //pdpt = (uint64_t*) 0x2000;
   //pageDir = (uint64_t*) 0x3000;
   //pageTable = (uint64_t*) 0x4000;

   //experimental
   //pageTable[1] = 

   //kprintf("PML4: %d\n", PML4[0]);
   //kprintf("pdpt: %d\n", pdpt[0]);
   //kprintf("PD: %d\n", pageDir[1]);
   //kprintf("PT: %d\n", pageTable[0]);

   freeMemAddr = 0x100000; //after 2MB we got free space,use this for allocing pages
   mapPage((uint64_t)0x100000, (uint64_t)0x100000, 0x0);
   kmallocFreeMem = 0x100000; //set it to the start of the now allocated page, it gets 1 page (4kb), if we need more we should allocate more
   allocEnd = 0x100FFF;

}

uint32_t kmalloc(uint32_t size, uint32_t *physAddr){
   //since physaddr is an empty pointer to where the allocated memory is located we just set it to where we have space
   
   if(physAddr) *physAddr = kmallocFreeMem;

   if (kmallocFreeMem + size >= allocEnd) { //>= because allocEnd points to the last alloc-able byte, which means it points to kmallocFreeMem + size + 1 if you try to allocate exactly how much space you have left

      mapPage(allocEnd + 1, allocEnd + 1, 0x0);
      allocEnd += 0x1000;
   
   }

   uint32_t ret = kmallocFreeMem; //dunno why we return the address twice but okay
   kmallocFreeMem += size; //move pointer to new free space
   kprintf("mem: %d\n", ret);
   return ret;

}
#define tempMem (uint64_t*)0x6000

//just find the next available page in free memory so we can use it to map stuff
void* alloc_page(void){

   void* page = (void*)freeMemAddr;
   freeMemAddr += 0x1000;
   return page;

}

uint8_t mapPage(uint64_t physAddr, uint64_t virtAddr, uint16_t flags){

   uint8_t p4idx = (uint8_t)virtAddr >> 39; //dont & because it has no effect
   uint8_t pdptidx = (uint8_t)virtAddr >> 30 & 0x1FF;
   uint8_t pdidx = (uint8_t)virtAddr >> 21 & 0x01FF;
   uint8_t ptidx = (uint8_t)virtAddr >> 12 & 0x01FF; //make sure everything is 0 except what we need
                                                     //
   kprintf("%i\n", p4idx);
   kprintf("%i\n", pdptidx);
   kprintf("%i\n", pdidx);
   kprintf("%i\n", ptidx);

  
   //*(tempMem) = p4idx;

   //gets triggered if there is no entry in the pml4
   if(!(PML4[p4idx]) & 0x01){

      uint64_t* pdpt = alloc_page(); //if it does not exist, allocate one
      //we should 0 the page but thats applications problems
      PML4[p4idx] = (uint64_t)pdpt | 0x01 | (0x01 << 1); //set the entry to contain the address to the pdpt that we allocated, the present bit and the writable bit

   }

   uint64_t* pdpt = (uint64_t*)(PML4[p4idx] & ~0xFFF); // the & should make sure we only grab address (~ means not)

   if (!(pdpt[pdptidx] & 0x01)) {
      
      uint64_t* pd = alloc_page(); //allocate because it's not there

      pdpt[pdptidx] = (uint64_t)pd | 0x01 | (0x01 << 1);


   }

   uint64_t* pd = (uint64_t*)(pdpt[pdptidx] & ~0xFFF);

   if (!(pd[pdidx] & 0x01)) {

      uint64_t* pt = alloc_page();
      pd[pdidx] = (uint64_t)pt | 0x01 | (0x01 << 1);
   
   }

   uint64_t* pt = (uint64_t*)(pd[pdidx] & ~0xFFF);

   pt[ptidx] = physAddr | flags | (uint64_t)0x01;



//   uint64_t* page = pt[ptidx];

   /*uint32_t pdinx = (uint32_t)virtAddr >> 22;
   uint32_t ptinx = (uint32_t)virtAddr >> 12 & 0x03FF;

   if(pdpt[pdinx] != 0x03){
     //check for present bit instead of != 0x03, if not present, make it 
   }

   uint32_t* pd = (uint32_t *)0xFFFFF000;

   uint32_t* pt = ((uint32_t *)0xFFC00000) + (0x400 * pdinx); */

   //pt[ptinx] = ((uint32_t)physAddr) | (flags & 0xFFF) | 0x01; //present

}
