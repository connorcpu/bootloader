#include "io.h"
#include "keyboard.h"
#include "ide.h"
#include "pic.h"
#include <stdint.h>
#include "fat.h"

extern int _start() {

   //make sure pic is godamn clear
   PIC_sendEOI(0x03);
   //init basic io vars for printing
   ioInit();
   //clear the screen
   cls();

   uint8_t test = 5;
   print("------------------------------------- C code ------------------------------------\0");
   print("connOS rules\n\0");
   putch('C', 79, 24);

   //clear interupts, setup interupts, enable interupts
   __asm__ volatile ("cli");
   createTable();
   __asm__ volatile ("sti");

   registerkdbint();
   //setup ide
   ideInit(0x1F0, 0x3F6, 0x170, 0x376, 0x000);

   kprintf("starting disk read...\n");
   uint8_t status = ide_read_sectors(0, 1, 0, 0x10, 0x80000);
   //read filesystems first sector (VBR)
   //uint8_t status = ide_read_sectors(0, 1, 0x000000800, 0x10, 0x70000);
   kprintf("disk read operation complete, status code: %d\n", status);

   fatInit();


   for(int i = 0; 1 == 1; i++){

      putch(i + 'A', 0, 1);

      if(i > 26){
         i = 0;
      }

   }

   return 0;
}

