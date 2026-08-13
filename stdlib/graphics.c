#include "graphics.h"
#include "math.h"

void drawRect(uint8_t _rgb[], uint8_t* where, uint16_t width, uint16_t height, uint8_t* buffer){

   where += (uint64_t)buffer;

   for(uint16_t j = 0; j < height; j++){

      for (uint16_t k = 0; k < width; k++) {
         
         //the existance of the above line is the sole fix for a totally unrelated bug, probably something with the stack.
         //please try comparing the different compilations

         where[k*3] = _rgb[0];
         where[k*3 + 1] = _rgb[1]; //this line causing issues
         where[k*3 + 2] = _rgb[2];
      
      }
         where += (1920*3);

   }

}

void drawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, uint8_t _rgb[], uint8_t* buffer){

   int64_t rise = y2 - y1;
   int64_t run  = x2 - x1;

   int64_t steps = abs(run) > abs(rise) ? abs(run) : abs(rise);

   if (!steps) steps++;

   float xinc = run / (float)steps;
   float yinc = rise / (float)steps;

   float x = x1;
   float y = y1;

   for(uint64_t i = 0; i <= steps; i++){

      putPixel(roundf(x), roundf(y), _rgb, buffer);
      x += xinc;
      y += yinc;
   }


}

void putPixelTrueSpace(int64_t x, int64_t y, uint8_t _rgb[], uint8_t* buffer){

   if(x < 0 || y < 0 || x > 1920 || y > 1080){return;}

   uint64_t where = (y * 3 * 1920) + (x * 3);
   buffer[where] = 255;
   buffer[where + 1] = 0;
   buffer[where + 2] = 255;

}

void putPixel(int64_t x, int64_t y, uint8_t _rgb[], uint8_t* buffer){

   putPixelTrueSpace(x+960, y+540, _rgb, buffer);

}
