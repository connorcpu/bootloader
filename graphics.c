#include "graphics.h"
#include <stdint.h>
#include "io.h"
#include "memory.h"
#include "interupt.h"

#define VGA_WIDTH (uint32_t) 1920
#define VGA_HEIGHT (uint32_t) 1080

const int playerSize = 20;
const int playerX = 20;
int playerY;  

extern VbeInfoStructure_t VbeInfoStructure;
extern VbeModeInfoStructure_t VbeModeInfoStructure;
uint16_t* vga_mem;

void drawRect(int x, int y, int width, int height, uint32_t colour){

   for (uint16_t j = y; j < height + y; j += 1) {
      for(uint16_t i = x; i < width + x; i += 1){
         putpixel((uint8_t*)vga_mem, i, j, colour);
      }
   }

   return;

}

void putPixel(uint16_t x, uint16_t y, uint32_t colour){

   putpixel((uint8_t*) vga_mem, x, y, colour);

}

void putpixel(uint8_t* screen, uint16_t x, uint16_t y, uint32_t colour){

   uint8_t* loc = (uint8_t*)screen + (y * VbeModeInfoStructure.width * 3) + (x*3) ;

   uint32_t* location = (uint32_t*)loc;
   *location = colour;

}

void interuptHandlerPutPixel(registers_t r){

   putPixel(r.rbx, r.rcx, r.rdx);

}

int initFrame(){

   //map the whole framebuffer
   for(int i = 0; i < 1519; i++){
      mapPage((uint8_t*)VbeModeInfoStructure.framebuffer + (i * 0x1000), (uint8_t*)0x2000000 + (i*0x1000), 0x0); //map "physical" video mem to 4GiB
   }

   vga_mem = (uint16_t *)0x2000000;

   registerInterupt(2, &interuptHandlerPutPixel);

   //drawRect(0, 0, 1920, 1080, RED);

   return 0;

}
