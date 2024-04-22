#include "io.h"
#include "interupt.h"
#include "keyboard.h"

extern int _start() {

   ioInit();
   createTable();
   PIC_remap(0x20, 0x28);

   //no, we want an interupt
   //__asm__ volatile ("syscall");
   //syscall();
   __asm__ volatile ("sti");

   print("------------------------------------- C code ------------------------------------\0");
   print("connOS rules\0");
   //print("test\0");
   //print("test\0");
   //print("test\0");
   //print("test\0");

   return 0;
}

