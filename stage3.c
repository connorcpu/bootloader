#include "io.h"
#include "keyboard.h"
#include "ide.h"
#include "pic.h"
#include <stdint.h>
#include "memory.h"
#include "ELF.h"
#include "graphics.h"

extern int _start() {

   //make sure pic is godamn clear
   PIC_sendEOI(0x03);
   //init basic io vars for printing
   ioInit();
   cls();

   print("------------------------------------- C code ------------------------------------\0");
   print("connOS rules\n\0");
   putch('C', 79, 24);

   //clear interupts, setup interupts, enable interupts
   __asm__ volatile ("cli");
   createTable();
   __asm__ volatile ("sti");

   //register keyboard interupt
   registerkdbint();

   //setup ide
   ideInit(0x1F0, 0x3F6, 0x170, 0x376, 0x000);

   //read MBR
   //uint8_t status = ide_read_sectors(0, 1, 0, 0x10, 0x80000);

   //init memory
   pagingInit();

   //initalize the fat
   fatInit();

   //initalize graphics
   initFrame();

 //  kprintf("hi\n");

   //test the filesystem
   //fileHeader_t* testFile = (fileHeader_t*) kmalloc(512, 0);
   //fileHeader_t* testFile = (fileHeader_t*) kmalloc(1280, 0);
   fileHeader_t* testFile = (fileHeader_t*) 0x0;
   openFile("test3.exe", testFile);
//   kprintf("opened executable\n");
   executeRaw(testFile);
//   openFile("test5.txt", testFile);
   //executeElf(testFile);

   for(int i = 0; 1 == 1; i++){

      putch(i + 'A', 0, 1);

      if(i > 26){
         i = 0;
      }

   }

   return 0;
}

