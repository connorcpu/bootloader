#include <stdint.h>
#include "stdGraphics.h"
#include "utils.h"

void drawRect(int x, int y, int width, int height, int colour){

   for (uint16_t j = y; j < height + y; j++) {
      for(uint16_t i = x; i < width + x; i++){

         //*(VGA_MEM + j * VGA_WIDTH + i) = colour;
         putPixel(x, y, colour);
      
      }

   }

   return;

}
