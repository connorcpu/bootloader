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

void io_wait(void){

   outb(0x80, 0);

}
