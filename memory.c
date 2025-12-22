#include "memory.h"
#include "io.h"
#include <stdint.h>

uint64_t* PML4;
//uint64_t* pdpt;
//uint64_t* pageDir;
//uint64_t* pageTable;

//uint64_t currentTable;
uint8_t* freeMemAddr;
uint8_t* kmallocFreeMem;
extern E820MemBlock memMap[256];
uint8_t* allocEnd; //stores the highest address we can alloced without having paging problems
//E820MemBlock memMap[10];

void pagingInit(){
/*
   uint8_t i = 0;
   while (memMap[i].length != 0) {
   
      kprintf("e820: found a region of type %d\n", memMap[i].type);
      kprintf("base: %d\n", memMap[i].base);
      kprintf("length: %d\n", memMap[i].length);

      i++;
   }*/


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

   freeMemAddr = (uint8_t*)memMap[3].base; //after 2MB we got free space,use this for allocing pages
   //mapPage((uint8_t*)0x100000, memMap[3].base, 0x0); //we have to map the page tables otherwise operating on the page table throws page fault :smh:
   mapPage((uint8_t*)0x1000000, (uint8_t*)0x1000000, 0x0);
   kmallocFreeMem = (uint8_t*)0x1000000; //set it to the start of the now allocated page, it gets 1 page (4kb), if we need more we should allocate more
   allocEnd = (uint8_t*)0x100FFF;

}

/*uint32_t kmalloc(uint32_t size, uint32_t *physAddr){
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

}*/

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

#define PAGE_ALIGN(x) ((x) & ~0xFFF)

   //physAddr = PAGE_ALIGN(physAddr);
   //virtAddr = PAGE_ALIGN(virtAddr);
   //

   uint16_t p4idx = ((uint64_t)virtAddr >> 39) & 0x1FF; //dont & because it has no effect
   uint16_t pdptidx = ((uint64_t)virtAddr >> 30) & 0x1FF;
   uint16_t pdidx = ((uint64_t)virtAddr >> 21) & 0x1FF;
   uint16_t ptidx = ((uint64_t)virtAddr >> 12) & 0x1FF; //make sure everything is 0 except what we need
                                                     //
/*   kprintf("4: %i\n", p4idx);
   kprintf("pdpt: %i\n", pdptidx);
   kprintf("pd: %i\n", pdidx);
   kprintf("pt: %i\n", ptidx);
*/
  
   //*(tempMem) = p4idx;

   //gets triggered if there is no entry in the pml4
   if(!(PML4[p4idx]) & 0x01){

      uint64_t* pdpt = alloc_page(); //if it does not exist, allocate one
      //we should 0 the page but thats applications problems
      PML4[p4idx] = (uint64_t)pdpt | 0x01 | (0x01 << 1); //set the entry to contain the address to the pdpt that we allocated, the present bit and the writable bit

   }

   uint64_t* pdpt = (uint64_t*)(PML4[p4idx] & ~0xFFF); // the & should make sure we only grab address (~ means not)
   //kprintf("pdpt: %d\n", pdpt);

   if (!(pdpt[pdptidx] & 0x01)) {
      
      uint64_t* pd = alloc_page(); //allocate because it's not there

      pdpt[pdptidx] = (uint64_t)pd | 0x01 | (0x01 << 1);


   }

   uint64_t* pd = (uint64_t*)(pdpt[pdptidx] & ~0xFFF);
   //kprintf("pd: %d\n", pd);

   if (!(pd[pdidx] & 0x01)) {

      uint64_t* pt = alloc_page();
      pd[pdidx] = (uint64_t)pt | 0x01 | (0x01 << 1);

   
   }

   uint64_t* pt = (uint64_t*)(pd[pdidx] & ~0xFFF);
   //kprintf("pt: %d\n", (uint64_t)pt);
   //kprintf("page contents: %d\n", pt[ptidx]);

   pt[ptidx] = (uint64_t)physAddr | (uint64_t)flags | (uint64_t)0x01 | (0x01 << 1);

   invlpg((void*)virtAddr);



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
