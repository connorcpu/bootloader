#include "graphics.h"
#include <stdint.h>

//#define VGA_WIDTH (uint16_t) 1280
//#define VGA_HEIGHT (uint16_t) 1024
//#define VGA_WIDTH (uint32_t) 640
//#define VGA_HEIGHT (uint32_t) 480
#define VGA_WIDTH (uint32_t) 1600 
#define VGA_HEIGHT (uint32_t) 1200
#define VGA_MEM (uint8_t*) 0xa0000

const int playerSize = 20;
const int playerX = 20;
int playerY;  

void drawRect(int x, int y, int width, int height, int colour){

   for(uint32_t i = x; i < width + x; i++){
      for (uint32_t j = y; j < height + y; j++) {

         *(VGA_MEM + j * VGA_WIDTH + i) = colour;
      
      }

   }

   return;

}

int initFrame(){

   playerY = VGA_HEIGHT / 2 - playerSize / 2;

  // drawRect(playerX, playerY, playerSize, playerSize, 12);

   for (uint32_t i = 0; i < 640; i++) {
      for (uint32_t j = 0; j < 480; j++) {

         //uint8_t* pixel = VGA_MEM + j * VGA_WIDTH + i;
         //*pixel = 55;
    //     *(VGA_MEM + j * VGA_WIDTH + i) = 15;

      }
   
   }

   for(uint32_t k = 0; k < 1600 * 1200; k++){

      uint8_t* pixel = VGA_MEM + k;
      *pixel = 55;

   }

   *((uint8_t*)0xb0000) = 24;
   uint32_t addr = 0xb0000;
   *(uint8_t *)addr = 24;
   *(uint8_t *)(addr + 1) = 24;


   return 0;

}
