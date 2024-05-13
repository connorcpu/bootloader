#include "io.h"
#include "interupt.h"
#include "keyboard.h"
#include "ide.h"

extern int _start() {

   //make sure pic is godamn clear
   PIC_sendEOI(0x03);
   //init basic io vars for printing
   ioInit();
   //clear the screen
   cls();

   //clear interupts, setup interupts, enable interupts
   __asm__ volatile ("cli");
   createTable();
   __asm__ volatile ("sti");

   //setup ide
//   ideInit(0x1F0, 0x3F6, 0x170, 0x376, 0x000);

   print("------------------------------------- C code ------------------------------------\0");
   print("connOS rules\0");
   putch('C', 89, 24);


   for(int i = 0; 1 == 1; i++){

      putch(i + 'A', 0, 1);

      if(i > 26){
         i = 0;
      }

   }

   return 0;
}

