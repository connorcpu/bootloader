#include "io.h"
#include "interupt.h"
#include "keyboard.h"

extern int _start() {

   PIC_sendEOI(0x03);
   ioInit();
   cls();
   __asm__ volatile ("cli");
   createTable();
   __asm__ volatile ("sti");

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

