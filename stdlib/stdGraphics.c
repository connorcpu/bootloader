#include <stdint.h>
#include "stdGraphics.h"

void drawRect(int x, int y, int width, int height, int colour){

   for(uint16_t i = x; i < width + x; i++){
      for (uint16_t j = y; j < height + y; j++) {

         *(VGA_MEM + j * VGA_WIDTH + i) = colour;
      
      }

   }

   return;

}
