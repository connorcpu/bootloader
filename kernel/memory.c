#include "memory.h"
#include "io.h"
#include <stdint.h>
#include "debug.h"
#include "utils.h"

extern VbeModeInfoStructure_t vbe;

uint64_t* higherPML4;

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

   //time to move everything to new, higherHalf paging tabel
   
   //this line works because the data is in both mappings at the same virtual address
   freeMemAddr = (uint8_t*)0x5000000;

   //allocate fresh pml4
   higherPML4 = alloc_page();
   
   //map some fresh pages so we can do more allocation
   for(uint16_t i = 0; i < 22; i++){
      mmPage((uint8_t*)0x5000000 + (i*0x1000),(uint8_t*)0x5000000 + (i*0x1000), 0x0, higherPML4);
   }

   //map kernel itself
   for(uint8_t i = 0; i < 10; i++){
      mmPage((uint8_t*)(0x6000000 + (i*0x1000)), (uint8_t*)(0xc0000000 + (i*0x1000)), 0x0, higherPML4);
   }

//   uint64_t toMap = ((VBEMIS.pitch * VBEMIS.height) / 0x1000) + 1;
   uint64_t toMap = (((1920*3)*1080)/0x1000)+1;
   kprintf("remapping framebuffer in %i pages at %h\n", toMap, vbe.framebuffer);
   for(int i = 0; i < toMap; i++){
 //     mmPage((uint8_t*)VBEMIS.framebuffer + (i * 0x1000), (uint8_t*)0x2000000 + (i*0x1000), 0x0, higherPML4); //map "physical" video mem to 4GiB
//      mmPage((uint8_t*)0xFD000000 + (i * 0x1000), (uint8_t*)0x2000000 + (i*0x1000), 0x0, higherPML4); //map "physical" video mem to 4GiB
    mmPage((uint8_t*)0xe0000000 + (i * 0x1000), (uint8_t*)0x2000000 + (i*0x1000), 0x0, higherPML4); //map "physical" video mem to 4GiB
//      mmPage((uint8_t*)vbe.framebuffer + (i * 0x1000), (uint8_t*)0x2000000 + (i*0x1000), 0x0, higherPML4); //map "physical" video mem to 4GiB
   }
   kprintf("remapped framebuffer\n");
   
   for(uint8_t i = 0; i < 18; i++){
      mmPage((uint8_t*)(0x1000*i) + 0x80000, (uint8_t*)(0x1000*i)+0x80000, 0x0, higherPML4);
   }
   kprintf("mapped old stack\n");

   __asm__ volatile ("mov %0, %%cr3" : : "r"(higherPML4));

   kprintf("switched to higher-half memory map\n");


   /*for(uint16_t i = 0; i < 20; i++){
      mapPage((uint8_t*)0x5001000 + (i*0x1000),(uint8_t*)0x5001000 + (i*0x1000), 0x0);
   }*/

//      mapPage((uint8_t*)0x50001000,(uint8_t*)0x50001000, 0x0);
   //just put empty memory at double the physical space of the kernel, and just C->D for virtual memory space; more than plenty
   //
   //skip this line because bootloader already did it for us
   //mapPage((uint8_t*)0xC000000, (uint8_t*)0xD0000000, 0x0);

   //mapPage((uint8_t*)0x100000, memMap[3].base, 0x0); //we have to map the page tables otherwise operating on the page table throws page fault :smh:
//   mapPage((uint8_t*)0x1000000, (uint8_t*)0x1000000, 0x0);
   
   //this for kmalloc space
   //lets fucking put at something that is also cannonical for 32-bit addressing so as to not trip up bochs :|
   mapPage((uint8_t*)0x4000000, (uint8_t*)0x60000000, 0x0);
   kmallocFreeMem = (uint8_t*)0x60000000; //set it to the start of the now allocated page, it gets 1 page (4kb), if we need more we should allocate more
   allocEnd = (uint8_t*)0x60000FFF;


   //just map some shit in advance, way simpler :(
   //needs to be rediculusly large because fat needs a rediculus amout of space (0.8 mb)
   for(uint8_t i = 0; i < 196; i++){
      mapPage((uint8_t*)0x60001000 + (i*0x1000),(uint8_t*)0x60001000 + (i*0x1000), 0x0);
   }

}

uint8_t* kmalloc(uint32_t size){

   if(kmallocFreeMem + size >= allocEnd){

      //this identity maps, that is wrong
//      kprintf("shiii\n");
      mapPage((uint8_t*)(allocEnd + 1), (uint8_t*)(allocEnd + 1), 0x0);
      allocEnd += 0x1000;

      /*while(kmalocFreeMem + size > allocEnd){
         mapPage((uint8_t*)(allocEnd + 1)

      }*/

      //((kmallocFreeMem + size) - allocEnd)

   }

   uint8_t* ret = kmallocFreeMem;
   kmallocFreeMem += size;
   kprintf("mem: allocated %h bytes at %h\n", size, ret);
   return ret;

}

#define tempMem (uint64_t*)0x6000

//just find the next available page in free memory so we can use it to map stuff
//so turns out that the addresses stored in the page structure are interpreted as physical addresses. therefore we are kinda forced to identity map the space where we allocate fresh pages
void* alloc_page(void){

   //this is where the next entries are stored you dummy
   //ofcourse it should be 0x1000 you need space to store the lower entries
   void* page = (void*)freeMemAddr;
   freeMemAddr += 0x1000;
   return page;

}


uint8_t mapPage(uint8_t* physAddr, uint8_t* virtAddr, uint16_t flags){

   mmPage(physAddr, virtAddr, flags, (void*)higherPML4);

}

uint8_t mmPage(uint8_t* physAddr, uint8_t* virtAddr, uint16_t flags, void* PML4addr){

#define PAGE_ALIGN(x) ((x) & ~0xFFF);

   uint64_t* PML4 = (uint64_t*) PML4addr; 

   physAddr = (uint8_t*)PAGE_ALIGN((uint64_t)physAddr);
   virtAddr = (uint8_t*)PAGE_ALIGN((uint64_t)virtAddr);
   //


   uint16_t p4idx = ((uint64_t)virtAddr >> 39) & 0x1FF; //dont & because it has no effect
   uint16_t pdptidx = ((uint64_t)virtAddr >> 30) & 0x1FF;
   uint16_t pdidx = ((uint64_t)virtAddr >> 21) & 0x1FF;
   uint16_t ptidx = ((uint64_t)virtAddr >> 12) & 0x1FF; //make sure everything is 0 except what we need
                                                     //
   /*kprintf("virt: %h\n", virtAddr);
   kprintf("4: %h\n", p4idx);
   kprintf("pdpt: %h\n", pdptidx);
   kprintf("pd: %h\n", pdidx);
   kprintf("pt: %h\n", ptidx);*/

   //kprintf("mem: mapping virt %h to phys %h\n", virtAddr, physAddr);

   //gets triggered if there is no entry in the pml4
   if(!(PML4[p4idx]) & 0x01){

    //  kprintf("pml4 was thi issue\n");
      uint64_t* pdpt = alloc_page(); //if it does not exist, allocate one
      //we should 0 the page but thats applications problems
      PML4[p4idx] = (uint64_t)pdpt | (uint64_t)(flags & 0xFFF) | 0x01 | (0x01 << 1); //set the entry to contain the address to the pdpt that we allocated, the present bit and the writable bit

   }else if(flags & 0xFFF){ //there are flags, we should prob update them

      PML4[p4idx] |= (uint64_t)(flags & 0xFFF);

   }

   uint64_t* pdpt = (uint64_t*)(PML4[p4idx] & ~0xFFF); // the & should make sure we only grab address (~ means not)
  //kprintf("pdpt: %h\n", pdpt);

   if (!(pdpt[pdptidx] & 0x01)) {
      //kprintf("pdpt was the issue\n");
      
      uint64_t* pd = alloc_page(); //allocate because it's not there

      pdpt[pdptidx] = (uint64_t)pd | (uint64_t)(flags & 0xFFF) | 0x01 | (0x01 << 1);


   }else if(flags & 0xFFF){ //there are flags, we should prob update them

      pdpt[pdptidx] |= (uint64_t)(flags & 0xFFF);

   }

   uint64_t* pd = (uint64_t*)(pdpt[pdptidx] & ~0xFFF);
   //kprintf("pd: %h\n", pd);

   if (!(pd[pdidx] & 0x01)) {
    //  kprintf("pd was the issue\n");

      uint64_t* pt = alloc_page();
      pd[pdidx] = (uint64_t)pt | (uint64_t)(flags & 0xFFF) | 0x01 | (0x01 << 1); //this line is not causing the GP faults

   
   }else if(flags & 0xFFF){ //there are flags, we should prob update them

      pd[pdidx] |= (uint64_t)(flags & 0xFFF);

   }

   uint64_t* pt = (uint64_t*)(pd[pdidx] & ~0xFFF);
   //kprintf("pt: %h\n", (uint64_t)pt);
   //kprintf("page contents: %d\n", pt[ptidx]);

   pt[ptidx] = (uint64_t)physAddr | (uint64_t)(flags & 0xFFF) | (uint64_t)0x01 | (0x01 << 1);

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
