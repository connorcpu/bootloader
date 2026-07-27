#include "memory.h"
#include "io.h"
#include <stdint.h>
#include "debug.h"
#include "utils.h"
#include <stddef.h>

extern VbeModeInfoStructure_t vbe;

uint64_t* higherPML4;

uint8_t* freeMemAddr;
uint8_t* kmallocFreeMem;
uint8_t* allocEnd; //stores the highest address we can alloced without having paging problems
void* physAlloc;
void* virtAlloc;

void pagingInit(){

   //time to move everything to new, higherHalf paging tabel
   
   //this line works because the data is in both mappings at the same virtual address
   freeMemAddr = (uint8_t*)0x5000000;

   //just define where to store physical and virtual pages
   physAlloc = (uint8_t*)0x200000;
   virtAlloc = (uint8_t*)0x100000000;

   //allocate fresh pml4
   higherPML4 = alloc_page();
   
   //map some fresh pages so we can do more allocation
   for(uint16_t i = 0; i < 32; i++){
      mmPage((void*)0x5000000 + (i*0x1000), (void*)0x5000000 + (i*0x1000), 0x0, higherPML4);
   }
   //freeMemAddr = mmap(0x16000, 0x0);

   //map kernel itself
   for(uint8_t i = 0; i < 10; i++){
      mmPage((void*)(uint64_t)(0x6000000 + (i*0x1000)), (void*)(uint64_t)(0xc0000000 + (i*0x1000)), 0x0, higherPML4);
   }

   //map framebuffer
   uint64_t toMap = (((1920*3)*1080)/0x1000)+1;
   kprintf("remapping framebuffer in %i pages at %h\n", toMap, vbe.framebuffer);
   for(int i = 0; i < toMap; i++){
      mmPage((void*)(uint64_t)vbe.framebuffer + (i * 0x1000), (void*)0x2000000 + (i*0x1000), 0x0, higherPML4); //map "physical" video mem to 4GiB
   }
   kprintf("remapped framebuffer\n");
   
   //map old stack
   for(uint8_t i = 0; i < 18; i++){
      mmPage((void*)(uint64_t)(0x1000*i) + 0x80000, (void*)(uint64_t)(0x1000*i)+0x80000, 0x0, higherPML4);
   }
   kprintf("mapped old stack\n");

   //load higher-half paging table
   __asm__ volatile ("mov %0, %%cr3" : : "r"(higherPML4));

   kprintf("switched to higher-half memory map\n");

   //this for kmalloc space
   //lets fucking put at something that is also cannonical for 32-bit addressing so as to not trip up bochs :|
   mapPage((void*)0x4000000, (void*)0x60000000, 0x0);
   kmallocFreeMem = (uint8_t*)0x60000000; //set it to the start of the now allocated page, it gets 1 page (4kb), if we need more we should allocate more
   //kmallocFreeMem = mmap(1, 0x0);
   allocEnd = (uint8_t*)0x60000FFF;
   //allocEnd = kmallocFreeMem + 0xFFF;

   kprintf("kmalloc: %h, allocEnd: %h\n", kmallocFreeMem, allocEnd);

}

uint8_t* kmalloc(uint32_t size){

      
   for(uint16_t i = 0; kmallocFreeMem + size >= allocEnd; i++){

      //damn, works like magic jeez
      mapPage((kmallocFreeMem + (0x1000*i) - 0x5c000000), (kmallocFreeMem + (0x1000 * i)), 0x0);
      allocEnd += 1000;

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


uint8_t mapPage(void* physAddr, void* virtAddr, uint16_t flags){

   mmPage(physAddr, virtAddr, flags, (void*)higherPML4);

}

uint8_t mmPage(void* physAddr, void* virtAddr, uint16_t flags, void* PML4addr){

#define PAGE_ALIGN(x) ((x) & ~0xFFF);

   uint64_t* PML4 = (uint64_t*) PML4addr; 

   physAddr = (uint8_t*)PAGE_ALIGN((uint64_t)physAddr);
   virtAddr = (uint8_t*)PAGE_ALIGN((uint64_t)virtAddr);

   uint16_t p4idx = ((uint64_t)virtAddr >> 39) & 0x1FF; //dont & because it has no effect
   uint16_t pdptidx = ((uint64_t)virtAddr >> 30) & 0x1FF;
   uint16_t pdidx = ((uint64_t)virtAddr >> 21) & 0x1FF;
   uint16_t ptidx = ((uint64_t)virtAddr >> 12) & 0x1FF; //make sure everything is 0 except what we need
                                                     //
   //kprintf("mem: mapping virt %h to phys %h\n", virtAddr, physAddr);

   //gets triggered if there is no entry in the pml4
   if(!(PML4[p4idx]) & 0x01){

      uint64_t* pdpt = alloc_page(); //if it does not exist, allocate one
      //we should 0 the page but thats applications problems
      PML4[p4idx] = (uint64_t)pdpt | (uint64_t)(flags & 0xFFF) | 0x01 | (0x01 << 1); //set the entry to contain the address to the pdpt that we allocated, the present bit and the writable bit

   }else if(flags & 0xFFF){ //there are flags, we should prob update them

      PML4[p4idx] |= (uint64_t)(flags & 0xFFF);

   }

   uint64_t* pdpt = (uint64_t*)(PML4[p4idx] & ~0xFFF); // the & should make sure we only grab address (~ means not)

   if (!(pdpt[pdptidx] & 0x01)) {
      
      uint64_t* pd = alloc_page(); //allocate because it's not there
      pdpt[pdptidx] = (uint64_t)pd | (uint64_t)(flags & 0xFFF) | 0x01 | (0x01 << 1);


   }else if(flags & 0xFFF){ //there are flags, we should prob update them

      pdpt[pdptidx] |= (uint64_t)(flags & 0xFFF);
 
   }

   uint64_t* pd = (uint64_t*)(pdpt[pdptidx] & ~0xFFF);

   if (!(pd[pdidx] & 0x01)) {

      uint64_t* pt = alloc_page();
      pd[pdidx] = (uint64_t)pt | (uint64_t)(flags & 0xFFF) | 0x01 | (0x01 << 1); //this line is not causing the GP faults

   
   }else if(flags & 0xFFF){ //there are flags, we should prob update them

      pd[pdidx] |= (uint64_t)(flags & 0xFFF);

   }

   uint64_t* pt = (uint64_t*)(pd[pdidx] & ~0xFFF);
   pt[ptidx] = (uint64_t)physAddr | (uint64_t)(flags & 0xFFF) | (uint64_t)0x01 | (0x01 << 1);

   //make sure to flush the lookaside table
   invlpg((void*)virtAddr);

}

void* mmap(size_t length, uint16_t flags){

   void* ret = virtAlloc;

   uint32_t toMap = (length / 0x1000) + 1;
   for(uint32_t i = 0; i < toMap; i++){

      mapPage((void*)physAlloc, (void*)virtAlloc, flags);
      kprintf("mapping virtual %h to physical %h\n", virtAlloc, physAlloc);

   }
   virtAlloc += 0x1000;
   physAlloc += 0x1000;

   return ret;

}

void* memcpy(void* dest, void* src, uint32_t size){

   uint8_t* d = (uint8_t*) dest;
   uint8_t* s = (uint8_t*) src;

   for(uint32_t i = 0; i < size; i++){

      d[i] = s[i];

   }
   return dest;

}
