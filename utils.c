#include "utils.h"

void outb(uint16_t port, uint8_t val){

   //both of these work i dont know which is better
   //__asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
   __asm__("out %%al, %%dx" : : "a" (val), "d" (port));

   return;

}

uint8_t inb(uint16_t port){

   uint8_t ret;
   
   //both of these work i dont know which is better
/*   __asm__ volatile ( "inb %w1, %b0"
                     : "=a"(ret)
                     : "Nd"(port)
                     : "memory");*/
   __asm__("in %%dx, %%al" : "=a" (ret) : "d" (port));
   return ret;

}

void insl(int32_t port, void *addr, int32_t cnt){

  asm volatile("cld; rep insl" : "=D" (addr), "=c" (cnt) : "d" (port), "0" (addr), "1" (cnt) : "memory", "cc"); 

}

void io_wait(void){

   outb(0x80, 0);

}
