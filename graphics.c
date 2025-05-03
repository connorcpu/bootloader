#include "graphics.h"
#include <stdint.h>
#include "io.h"

//#define VGA_WIDTH (uint16_t) 1280
//#define VGA_HEIGHT (uint16_t) 1024
//#define VGA_WIDTH (uint32_t) 640
//#define VGA_HEIGHT (uint32_t) 480
//#define VGA_WIDTH (uint32_t) 1600 
//#define VGA_HEIGHT (uint32_t) 1200
//#define VGA_WIDTH (uint32_t) 800 
//#define VGA_HEIGHT (uint32_t) 600
#define VGA_WIDTH (uint32_t) 320 
#define VGA_HEIGHT (uint32_t) 200
#define VGA_MEM (uint8_t*) 0xa0000

const int playerSize = 20;
const int playerX = 20;
int playerY;  

extern VbeInfoStructure_t VbeInfoStructure;
extern VbeModeInfoStructure_t VbeModeInfoStructure;
uint8_t* vga_mem;

void drawRect(int x, int y, int width, int height, int colour){

   for(uint16_t i = x; i < width + x; i++){
      for (uint16_t j = y; j < height + y; j++) {

         *(VGA_MEM + j * VGA_WIDTH + i) = colour;
      
      }

   }

   return;

}

void putpixel(uint8_t* screen, uint16_t x, uint16_t y, uint8_t colour){

  unsigned where = x * 3 + y * 3 * VGA_WIDTH; 
  screen[where] = colour & 255; //BLUE
  screen[where + 1] = (colour >> 8) & 255;
  screen[where + 2] = (colour >> 16) & 255;

}

int initFrame(){

   vga_mem = (uint8_t *)VbeModeInfoStructure.framebuffer;

  /* kprintf("width: %i\n", &VbeModeInfoStructure.width);
   kprintf("height: %i\n", &VbeModeInfoStructure.height);
   kprintf("framebuffer: %i\n", &VbeModeInfoStructure.framebuffer);
   kprintf("bpp: %i\n", &VbeModeInfoStructure.bpp); 
   kprintf("width: %i\n", VbeModeInfoStructure.width);
   kprintf("height: %i\n", VbeModeInfoStructure.height);
   kprintf("framebuffer: %i\n", VbeModeInfoStructure.framebuffer);
   kprintf("bpp: %i\n", VbeModeInfoStructure.bpp); 
   kprintf("vbe addr: %d\n", &VbeInfoStructure);
   kprintf("vbe addr: %d\n", &VbeModeInfoStructure);*/
   //kprintf("vbe: %i\n", VbeInfoStructure.video_modes[0]);
   //kprintf("vbe2: %i\n", VbeInfoStructure.video_modes[1]);

   uint16_t* modes = (uint16_t* )(VbeInfoStructure.video_modes[0] * 0x10 + VbeInfoStructure.video_modes[1]);

//   kprintf("modeArray: %d\n", modes);
//   kprintf("modebuffer: %i\n", vga_mem);


   //drawRect(0, 0, 100, 100, 55);
/*
   for (uint16_t i = 0; i < VGA_WIDTH; i++) {
      for (uint16_t j = 0; j < VGA_HEIGHT; j++) {

         //this one works
         uint8_t* pixel = VGA_MEM + j * VGA_WIDTH + i;
         *pixel = 55;
         //*(vga_mem + j * VGA_WIDTH + i) = 15;
         //*((uint8_t *)0xa0000 + j * VGA_WIDTH + i) = 15;
         //putpixel(VGA_MEM, i, j, 15);

      }
   
   }

   /*for(uint32_t k = 0x0FC000; k > 0xa0000; k--){

      //kprintf("pixel: %d\n", k);

      uint8_t* pixel = vga_mem + k;
      *pixel = 5;

   }*/

   *((uint8_t*)0xb0000) = 24;
   uint32_t addr = 0xb0000;
   *(uint8_t *)addr = 24;
   *(uint8_t *)(addr + 1) = 24;


   return 0;

}
