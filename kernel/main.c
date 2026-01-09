#include <stdint.h>
//#include "io.h"
//#include "debug.h"

typedef struct bootArgs {
   
   uint64_t VBEInfoBlockAddr;
   uint64_t E820Addr;
   uint64_t kernelPML4Addr; 

}__attribute__ ((packed)) bootArgs_t;

void drawRect(uint8_t rgb[]);

int _start(){

   __asm__ volatile ("xchg %bx, %bx");

   //kprintf("kernel loaded");

   //print("kernel loaded");
   
   //writeSerial('Z');

   uint8_t rgb[3];
   rgb[0] = 255;
   rgb[1] = 0;
   rgb[2] = 0;
   drawRect(rgb);

   for(uint8_t i = 0; i < 50; i++){

      for(uint8_t j = 0; j < 255; j++){
         rgb[1] = j;
         drawRect(rgb);
      }

      for(uint8_t k = 255; k > 0; k--){
         rgb[0] = k;
         drawRect(rgb);
      }

      for(uint8_t l = 0; l < 255; l++){
         rgb[2] = l;
         drawRect(rgb);
      }

      for(uint8_t m = 255; m > 0; m--){
         rgb[1] = m;
         drawRect(rgb);
      }

      for(uint8_t n = 0; n < 255; n++){
         rgb[0] = n;
         drawRect(rgb);
      }

      for(uint8_t o = 255; o > 0; o--){
         rgb[2] = o;
         drawRect(rgb);
      }
   
      //if(i >= 4) i = 0;

   }

   
   


   return 0;


}

void drawRect(uint8_t _rgb[]){

   uint8_t* volatile vga_mem = (uint8_t *)0x2000000;
   uint8_t* where = vga_mem;

   for(uint8_t j = 0; j < 100; j++){

      for (uint8_t k = 0; k < 100; k++) {

         /*uint8_t* loc = (uint8_t *)vga_mem + (j * 3 * 1920) + (k * 3);
         *loc = _rgb[0];
         *(loc+1) = _rgb[1];
         *(loc+2) = _rgb[2];*/

         where[k*3] = _rgb[0];
         where[k*3 + 1] = _rgb[1];
         where[k*3 + 2] = _rgb[2];

      
      }
         where += (1920*3);

   }
}
