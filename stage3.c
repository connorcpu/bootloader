#include "io.h"
#include "interupt.h"
#include "keyboard.h"
#include <stdint.h>

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

   __asm__ volatile ("xor %edi, %edi; div %edi");
   //__asm__ volatile ("cli; hlt");

   print("test\0");
   //print("test\0");
   //print("test\0");
   //print("test\0");

   return 0;
}

