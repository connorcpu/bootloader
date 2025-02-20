#include <stdint.h>

#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define VGA_MEM (uint8_t*)0xa0000

uint8_t _colour = 25;

void drawRect(int x, int y, int width, int height, int colour);
int _start(){

   drawRect(0, 0, VGA_WIDTH, VGA_HEIGHT, _colour);

   return 0;

}

void putpixel(uint8_t* screen, uint16_t x, uint16_t y, uint8_t colour){

  unsigned where = x * 3 + y * 3 * VGA_WIDTH; 
  screen[where] = colour & 255; //BLUE
  screen[where + 1] = (colour >> 8) & 255;
  screen[where + 2] = (colour >> 16) & 255;

  return;

}

void drawRect(int x, int y, int width, int height, int colour){


   for(uint16_t i = x; i < width; i++){
      for (uint16_t j = y; j < height; j++) {

         *(VGA_MEM + j * VGA_WIDTH + i) = _colour;
            //putpixel(VGA_MEM, i, j, colour);
         if(_colour >= 255){
            _colour = 0;
            continue;
         }
         _colour++;
         continue;

      }

   }

   return;

}

