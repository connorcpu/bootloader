#include <stdint.h>
#include "io.h"

typedef struct bootArgs {
   
   uint64_t VBEInfoBlockAddr;
   uint64_t E820Addr;
   uint64_t kernelPML4Addr; 

}__attribute__ ((packed)) bootArgs_t;

int _start(){

   //kprintf("kernel loaded");

   __asm__ volatile ("xchg %bx, %bx");

   uint16_t* volatile vga_mem = (uint16_t *)0x2000000;

   for(uint32_t i = 0; i < 16777216; i++){

      for(uint16_t j = 0; j < 100; j++){

         for (uint16_t k = 0; k < 100; k++) {

            uint16_t* loc = (uint16_t *)vga_mem + (j * 1920) + k;
            *loc = i;
         
         }


      }

   }


   return 0;


}
