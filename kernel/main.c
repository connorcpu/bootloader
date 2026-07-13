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
   //__asm__ volatile ("mov $0xbFFFFFFF, %rsp");
   //__asm__ volatile ("mov %rsp, %rbp");

   kprintf("ker: kernel loaded\n");

   kprintf("ker: pml4: %h\n", args.kernelPML4Addr);

   kprintf("vga: %h\n", args.VBEInfoBlockAddr);

   vbe = *((VbeModeInfoStructure_t*)arguments.VBEInfoBlockAddr);
   //vga_mem = (uint8_t*)vbe.framebuffer;
   vga_mem = (uint8_t*)arguments.framebuffer;

   PIC_sendEOI(0x03);

   createIDT();

   pagingInit();

   kprintf("mapping stack\n");
   for(uint8_t i = 5; i > 0; i--){
   
      mapPage((uint8_t*)(0x6000000 - (i*0x1000)), (uint8_t*)(0xc0000000-(i*0x1000)), 0x0);
      kprintf("%i: mapping %h, to %h\n", i, (0xc0000000-(i*0x1000)), (0x6000000-(i*0x1000)));

   }

   arguments = args;

   kprintf("switching stacks\n");
   //bochsBreak();

   //make sure not to declare local vars before this point to ensure stack transitions propperly
   //__asm__ volatile ("mov $0xBFFFFF00, %rsp");
   //__asm__ volatile ("mov %rsp, %rbp");


   kprintf("ker: loading GDT\n");

   loadGDT();

   kprintf("tss: %h\n", tss);

   ideInit(0x1F0, 0x3F6, 0x170, 0x376, 0x000);

   fatInit();

   startfd();

   __asm__ volatile("sti");
   
   setupSyscall(arguments.VBEInfoBlockAddr);

   mapPage((uint8_t*)0x2000000, (uint8_t*)0xD000000, 0x0);
   mapPage((uint8_t*)0x2001000, (uint8_t*)0xD001000, 0x0);
   mapPage((uint8_t*)0x2002000, (uint8_t*)0xD002000, 0x0);
   fileHeader_t* exef = (fileHeader_t*) 0xD000000;
   if(openFile("syscall.elf", exef) == -1){
      kprintf("ker: did not find syscall test file\n");
   }else{

      kprintf("ker: testing syscalls\n");

      executeElf(exef);

      kprintf("ker: returned to kernel code\n");
   }
   idle();
   return 0;
}

void idle(){

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
   
      //if(i >= 4) i = 0;

   }

   return;


}

void drawRect(uint8_t _rgb[]){

   uint8_t* volatile vga_mem = (uint8_t *)0x2000000;
   uint8_t* where = vga_mem;
   //uint8_t* where = (uint8_t*)0x2000000;

   for(uint16_t j = 0; j < 200; j++){

      for (uint16_t k = 0; k < 300; k++) {

         where[k*3] = _rgb[0];
         where[k*3 + 1] = _rgb[1];
         where[k*3 + 2] = _rgb[2];
      
      }
         where += (1920*3);

   }
}
