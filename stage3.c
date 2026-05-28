#include "io.h"
#include "debug.h"
#include "keyboard.h"
#include "ide.h"
#include "pic.h"
#include <stdint.h>
#include "memory.h"
#include "ELF.h"
#include "graphics.h"

typedef struct bootArgs {

   uint64_t VBEInfoBlockAddr;
   uint64_t E820Addr;
   uint64_t kernelPML4Addr;

}__attribute__((packed)) bootArgs_t;

extern int _start() {

   //make sure pic is godamn clear
//   PIC_sendEOI(0x03);
   //init basic io vars for printing
   ioInit();
   cls();

   print("------------------------------------- C code ------------------------------------\0");
   print("connOS rules\n\0");
   putch('C', 79, 24);

   //clear interupts, setup interupts, enable interupts
   __asm__ volatile ("cli");
  // createTable();
   __asm__ volatile ("sti");

   //register keyboard interupt
 //  registerkdbint();

   //setup ide
   ideInit(0x1F0, 0x3F6, 0x170, 0x376, 0x000);

   //read MBR
   //uint8_t status = ide_read_sectors(0, 1, 0, 0x10, 0x80000);
   initSerial();

   //init memory
   pagingInit();

   //initalize the fat
   fatInit();

   //initalize graphics
   initFrame();

   //mapping pages for kernel
   for(int i = 0; i < 9; i++){
      mapPage((uint8_t*)(0x6000000 + (i*0x1000)), (uint8_t*)(0xC0000000 + (i*0x1000)), 0x0);
      kprintf("%i: mappping page at phys: %h, to virt: %h\n", i, (0x6000000 + (i*0x1000)), (0xc0000000 + (i*0x1000)));
   }

   //cheating to make it easier
   //this is where we put new pages in the kernel, has to be identity mapped or go with the complicated route (future)
   mapPage((uint8_t*)0x5000000, (uint8_t*)0x5000000, 0x0);
   mapPage((uint8_t*)0x50001000,(uint8_t*)0x50001000, 0x0);

   kmalloc(8);

   //picking kernel load location
   fileHeader_t* testFile = (fileHeader_t*) 0xC0000000;

   kprintf("opening file\n");

   if(openFile("kernel.bin", testFile) == -1){
      kprintf("could not load kernel from disk, terminating...\n");
      return -1;   
   }

   //debug
   kprintf("opened executable\n");

   //setup kernel args
   bootArgs_t argsB;
   argsB.kernelPML4Addr = (uint64_t) PML4ADDR;
   //argsB.Vbe

   //run kernel
   //executeRaw(testFile);
   void (*kernel)(bootArgs_t) = (void (*)(bootArgs_t args))testFile;
   //__asm__ volatile ("mov $0xBFFFFF00, %rsp");
   kernel(argsB);

   //catch loop
   for(int i = 0; 1 == 1; i++){

      putch(i + 'A', 0, 1);

      if(i > 26){
         i = 0;
      }

   }

   return 0;
}

