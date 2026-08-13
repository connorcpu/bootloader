#include <stdint.h>
#include "debug.h"
#include "io.h"
#include "GDT.h"
#include "ELF.h"
#include "syscall.h"
#include "memory.h"
#include "ide.h"
#include "interrupt.h"
#include "PIC.h"
#include "syscalls/fileDescriptor.h"
#include "utils.h"
#include "tss.h"
#include "PCI.h"

typedef struct bootArgs {
   
   uint64_t VBEInfoBlockAddr;
   uint64_t E820Addr;
   uint64_t kernelPML4Addr; 
   uint64_t framebuffer;

}__attribute__ ((packed)) bootArgs_t;

bootArgs_t arguments;

VbeModeInfoStructure_t vbe;

void drawRect(uint8_t rgb[]);
void idle();
uint8_t* vga_mem;

extern tss_t tss;

int _start(bootArgs_t args){

   __asm__ volatile ("xchg %bx, %bx");

   kprintf("ker: kernel loaded\n");

   kprintf("ker: pml4: %h\n", args.kernelPML4Addr);

   //for global usuage
   vbe = *((VbeModeInfoStructure_t*)args.VBEInfoBlockAddr);

   //for local usage
   vga_mem = (uint8_t*)arguments.framebuffer;

   PIC_sendEOI(0x03);

   createIDT();

   pagingInit();
   bochsBreak();

   uint64_t _cr0;
   uint64_t _cr4;
   __asm__ volatile("movq %%cr0, %0" : "=r"(_cr0) :: );
   __asm__ volatile("movq %%cr4, %0" : "=r"(_cr4) :: );
   kprintf("cr4: %h\ncr0: %h\n", _cr4, _cr0);
   _cr0 &= ~(1 << 2);
   _cr0 |= (1 << 1);
   _cr4 |= (1 << 9);
   _cr4 |= (1 << 10);
   asm volatile("mov %0, %%cr0" :: "r"((uint64_t)_cr0): "memory");
   asm volatile("mov %0, %%cr4" :: "r"((uint64_t)_cr4): "memory");

   /*__asm__ volatile ("movq %%cr0, %%rax\t\n"
         "andb 0xFB, %%al\t\n"
         "or 0x2, %%ax\t\n"
         "mov %%rax, %%cr0\t\n"
         "mov %%cr4, %%rax\t\n"
         "or (3 << 9), %%ax\t\n"
         "mov %%rax, %%cr4\t\n" 
         :::);*/

   kprintf("mapping stack\n");

   for(uint8_t i = 5; i > 0; i--){
   
      mapPage((uint8_t*)(uint64_t)(0x6000000 - (i*0x1000)), (uint8_t*)(uint64_t)(0xc0000000-(i*0x1000)), 0x0);
      kprintf("%i: mapping %h, to %h\n", i, (0xc0000000-(i*0x1000)), (0x6000000-(i*0x1000)));

   }

   arguments = args;

   kprintf("switching stacks\n");

   //make sure not to declare local vars before this point to ensure stack transitions propperly
   __asm__ volatile ("mov $0xBFFFFF00, %rsp");
   __asm__ volatile ("mov %rsp, %rbp");


   kprintf("ker: loading GDT\n");

   loadGDT();

   ideInit(0x1F0, 0x3F6, 0x170, 0x376, 0x000);

   fatInit();

   startfd();

   __asm__ volatile("sti");
   
   setupSyscall(arguments.VBEInfoBlockAddr);

   pciDetectAll();
   
   kprintf("KERNEL: initialization complete, have fun\n");

   //finished setting up


   loadFile("/objects/cube.obj");

   //__asm__ volatile ("cli\t\nhlt");
   //due to compiler BS, there HAS, to be a line of code between 2 loadFile statements :)
   kprintf("loading renderer\n");

   //starting tests
   if(loadElf("/renderer.elf") == -1){
      kprintf("ker: error during elf loading\n");
   }




   //idling
   idle();
   return 0;

}

void idle(){

   kprintf("KERNEL: staring idle\n");

   uint8_t rgb[3];
   rgb[0] = 255;
   rgb[1] = 0;
   rgb[2] = 0;
   drawRect(rgb);

   for(uint8_t i = 0; i < 50; i++){

      for(uint8_t j = 0; j < 255; j++){
         rgb[1] = j;
         drawRect(rgb);
      }

      for(uint8_t k = 255; k > 0; k--){
         rgb[0] = k;
         drawRect(rgb);
      }

      for(uint8_t l = 0; l < 255; l++){
         rgb[2] = l;
         drawRect(rgb);
      }

      for(uint8_t m = 255; m > 0; m--){
         rgb[1] = m;
         drawRect(rgb);
      }

      for(uint8_t n = 0; n < 255; n++){
         rgb[0] = n;
         drawRect(rgb);
      }

      for(uint8_t o = 255; o > 0; o--){
         rgb[2] = o;
         drawRect(rgb);
      }
   
   }

   return;


}

void drawRect(uint8_t _rgb[]){

   uint8_t* volatile vga_mem = (uint8_t *)0x2000000;
   uint8_t* where = vga_mem;

   for(uint16_t j = 0; j < 200; j++){

      for (uint16_t k = 0; k < 300; k++) {

         where[k*3] = _rgb[0];
         where[k*3 + 1] = _rgb[1];
         where[k*3 + 2] = _rgb[2];
      
      }
         where += (1920*3);

   }
}
