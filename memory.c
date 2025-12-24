#include "memory.h"
#include "io.h"
#include <stdint.h>

uint64_t* PML4;
uint8_t* freeMemAddr;
uint8_t* kmallocFreeMem;
extern E820MemBlock memMap[256];
uint8_t* allocEnd; //stores the highest address we can alloced without having paging problems

void pagingInit(){

   PML4 = (uint64_t*) PML4ADDR; 

   freeMemAddr = (uint8_t*)memMap[3].base; //after 2MB we got free space,use this for allocing pages
   mapPage((uint8_t*)0x1000000, (uint8_t*)0x1000000, 0x0);
   kmallocFreeMem = (uint8_t*)0x1000000; //set it to the start of the now allocated page, it gets 1 page (4kb), if we need more we should allocate more
   allocEnd = (uint8_t*)0x100FFF;

}

uint8_t* kmalloc(uint32_t size){

   if(kmallocFreeMem + size >= allocEnd){

      mapPage((uint8_t*)(allocEnd + 1), (uint8_t*)(allocEnd + 1), 0x0);
      allocEnd += 0x1000;

   }

   uint8_t* ret = kmallocFreeMem;
   kmallocFreeMem += size;
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

uint8_t mapPage(uint8_t* physAddr, uint8_t* virtAddr, uint16_t flags){

  mappage(PML4, physAddr, virtAddr, flags); 

}

uint8_t mappage(uint64_t* pml4Ptr, uint8_t* physAddr, uint8_t* virtAddr, uint16_t flags){

#define PAGE_ALIGN(x) ((x) & ~0xFFF)

   uint16_t p4idx = ((uint64_t)virtAddr >> 39) & 0x1FF; //dont & because it has no effect
   uint16_t pdptidx = ((uint64_t)virtAddr >> 30) & 0x1FF;
   uint16_t pdidx = ((uint64_t)virtAddr >> 21) & 0x1FF;
   uint16_t ptidx = ((uint64_t)virtAddr >> 12) & 0x1FF; //make sure everything is 0 except what we need
                                                     //
   //gets triggered if there is no entry in the pml4
   if(!(pml4Ptr[p4idx]) & 0x01){

      uint64_t* pdpt = alloc_page(); //if it does not exist, allocate one
      //we should 0 the page but thats applications problems
      pml4Ptr[p4idx] = (uint64_t)pdpt | 0x01 | (0x01 << 1); //set the entry to contain the address to the pdpt that we allocated, the present bit and the writable bit

   }

   uint64_t* pdpt = (uint64_t*)(pml4Ptr[p4idx] & ~0xFFF); // the & should make sure we only grab address (~ means not)

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

   pt[ptidx] = (uint64_t)physAddr | (uint64_t)flags | (uint64_t)0x01 | (0x01 << 1);

   invlpg((void*)virtAddr);

}
