#include "write.h"
#include "fileDescriptor.h"
#include "../io.h"
#include "../memory.h"
#include <stdint.h>
#include "../debug.h"

void sysWrite(uint64_t rdi, uint64_t rsi, uint64_t rdx){

   //1. parse ASM registers to params (that did not work so now they are passed
   //2. filter out 0, 1 and 2
   //3. fetch filedescs

   //1.
   uint8_t* buf = (uint8_t*)rsi;
   uint8_t fd = (uint8_t)rdi;
   uint64_t size = rdx;

   kprintf("sys: going for buf: %h, at size: %d to fd: %d\n", buf, size, fd);

   //2.
   switch(fd){
      case 0:
         //stdin
         kprintf("sys: input not implemented\n");
         break;
      case 1:
         //stdout
         for(uint64_t i = 0; i < size; i++){ putch(*(buf+i)); }
         break;
      case 2:
         //stderr
         for(uint64_t i = 0; i < size; i++){ kprintf("err: "); putch(*(buf+i)); }
         break;
      default:
         //3. (not implemented)
         kprintf("sys: fd %d not found \n", fd);
   }
   return;
}
