#include "memory.h"
#include "io.h"
#include <stdint.h>
#include "debug.h"

uint64_t* PML4;

uint8_t* freeMemAddr;
uint8_t* kmallocFreeMem;
//extern E820MemBlock memMap[256];
uint8_t* allocEnd; //stores the highest address we can alloced without having paging problems

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

   freeMemAddr = (uint8_t*)0x5000000;
   //just put empty memory at double the physical space of the kernel, and just C->D for virtual memory space; more than plenty
   //
   //skip this line because bootloader already did it for us
   //mapPage((uint8_t*)0xC000000, (uint8_t*)0xD0000000, 0x0);

   //mapPage((uint8_t*)0x100000, memMap[3].base, 0x0); //we have to map the page tables otherwise operating on the page table throws page fault :smh:
//   mapPage((uint8_t*)0x1000000, (uint8_t*)0x1000000, 0x0);
   
   //this for kmalloc space
   mapPage((uint8_t*)0x4000000, (uint8_t*)0xE0000000, 0x0);
   kprintf("mapped the bloody page\n");
   kmallocFreeMem = (uint8_t*)0xe0000000; //set it to the start of the now allocated page, it gets 1 page (4kb), if we need more we should allocate more
   allocEnd = (uint8_t*)0xe0000FFF;


   //just map some shit in advance, way simpler :(
/*   for(uint8_t i = 0; i < 3; i++){
      mapPage((uint8_t*)0xe0001000 + (i*0x1000),(uint8_t*)0xe0001000 + (i*0x1000), 0x0);
   }*/

}

uint8_t* kmalloc(uint32_t size){
   kprintf("loc'ing %d bytes at %h\n", size, kmallocFreeMem);

   if(kmallocFreeMem + size >= allocEnd){

      //this identity maps, that is wrong
      kprintf("shiii\n");
      mapPage((uint8_t*)(allocEnd + 1), (uint8_t*)(allocEnd + 1), 0x0);
      allocEnd += 0x1000;

   }

   uint8_t* ret = kmallocFreeMem;
   kmallocFreeMem += size;
   kprintf("allocated\n");
   return ret;

}

#define tempMem (uint64_t*)0x6000

//just find the next available page in free memory so we can use it to map stuff
//so turns out that the addresses stored in the page structure are interpreted as physical addresses. therefore we are kinda forced to identity map the space where we allocate fresh pages
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
   kprintf("virt: %h\n", virtAddr);
   kprintf("4: %h\n", p4idx);
   kprintf("pdpt: %h\n", pdptidx);
   kprintf("pd: %h\n", pdidx);
   kprintf("pt: %h\n", ptidx);

  

   //gets triggered if there is no entry in the pml4
   if(!(PML4[p4idx]) & 0x01){

      kprintf("pml4 was thi issue\n");
      uint64_t* pdpt = alloc_page(); //if it does not exist, allocate one
      //we should 0 the page but thats applications problems
      PML4[p4idx] = (uint64_t)pdpt | 0x01 | (0x01 << 1); //set the entry to contain the address to the pdpt that we allocated, the present bit and the writable bit

   }

   kprintf("1\n");

   uint64_t* pdpt = (uint64_t*)(PML4[p4idx] & ~0xFFF); // the & should make sure we only grab address (~ means not)
   kprintf("pdpt: %h\n", pdpt);

   if (!(pdpt[pdptidx] & 0x01)) {
      kprintf("pdpt was the issue\n");
      
      uint64_t* pd = alloc_page(); //allocate because it's not there

      pdpt[pdptidx] = (uint64_t)pd | 0x01 | (0x01 << 1);


   }
   kprintf("2\n");

   uint64_t* pd = (uint64_t*)(pdpt[pdptidx] & ~0xFFF);
   kprintf("pd: %h\n", pd);

   if (!(pd[pdidx] & 0x01)) {
      kprintf("pd was the issue\n");

      uint64_t* pt = alloc_page();
      pd[pdidx] = (uint64_t)pt | 0x01 | (0x01 << 1);

   
   }
   kprintf("3\n");

   uint64_t* pt = (uint64_t*)(pd[pdidx] & ~0xFFF);
   kprintf("pt: %h\n", (uint64_t)pt);
   //kprintf("page contents: %d\n", pt[ptidx]);

   pt[ptidx] = (uint64_t)physAddr | (uint64_t)flags | (uint64_t)0x01 | (0x01 << 1);
   kprintf("4\n");
   kprintf("should say \"0xe000003\": %h\n", pt[ptidx]);
   kprintf("physAddr: %h\n", physAddr);

   invlpg((void*)virtAddr);

}

void* memcpy(void* dest, void* src, uint32_t size){

   uint8_t* d = (uint8_t*) dest;
   uint8_t* s = (uint8_t*) src;

   for(uint32_t i = 0; i < size; i++){

      d[i] = s[i];

   }
   return dest;

}
