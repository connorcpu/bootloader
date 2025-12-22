#include "graphics.h"
#include <stdint.h>
#include "io.h"
#include "memory.h"

//#define VGA_WIDTH (uint16_t) 1280
//#define VGA_HEIGHT (uint16_t) 1024
//#define VGA_WIDTH (uint32_t) 640
//#define VGA_HEIGHT (uint32_t) 480
//#define VGA_WIDTH (uint32_t) 1600 
//#define VGA_HEIGHT (uint32_t) 1200
//#define VGA_WIDTH (uint32_t) 800 
//#define VGA_HEIGHT (uint32_t) 600
#define VGA_WIDTH (uint32_t) 1920
#define VGA_HEIGHT (uint32_t) 1080
#define VGA_MEM (uint8_t*) 0xa0000
#define tempMem (uint32_t*) 0x6000

const int playerSize = 20;
const int playerX = 20;
int playerY;  

extern VbeInfoStructure_t VbeInfoStructure;
extern VbeModeInfoStructure_t VbeModeInfoStructure;
uint16_t* vga_mem;

void drawRect(int x, int y, int width, int height, int colour){

   //for (uint16_t j = y; j < height + y; j += VbeModeInfoStructure.bpp) {
     // for(uint16_t i = x; i < width + x; i += VbeModeInfoStructure.bpp){

   for (uint16_t j = y; j < height + y; j += 1) {
      for(uint16_t i = x; i < width + x; i += 1){
         //*(VGA_MEM + (j * VGA_WIDTH * 2) + (i * 2)) = colour;
         putpixel((uint32_t*)VbeModeInfoStructure.framebuffer, i, j, colour);

      }

   }

   return;

}

void putpixel(uint32_t* screen, uint16_t x, uint16_t y, uint8_t colour){

   //uint8_t* loc = (uint8_t*)screen + (y * VbeModeInfoStructure.width * VbeModeInfoStructure.bpp) + (x * VbeModeInfoStructure.bpp);
   uint8_t* loc = (uint8_t*)0xa0000 + (y * 1920 * 2) + (x * 2); //TODO: VbeModeInfoStructure.framebuffer is not populated correctly
   *loc = colour;

}

int initFrame(){

   vga_mem = (uint16_t *)VbeModeInfoStructure.framebuffer;

   kprintf("framebuffer: %i\n", (uint32_t)VbeModeInfoStructure.framebuffer);
   //kprintf("width: %i\n", &VbeModeInfoStructure.width);
   //kprintf("height: %i\n", &VbeModeInfoStructure.height);
   //kprintf("framebuffer: %i\n", &VbeModeInfoStructure.framebuffer);
   //kprintf("bpp: %i\n", &VbeModeInfoStructure.bpp); 
   kprintf("width: %i\n", VbeModeInfoStructure.width);
   kprintf("height: %i\n", VbeModeInfoStructure.height);
   kprintf("bpp: %i\n", VbeModeInfoStructure.bpp); 
   //kprintf("vbe addr: %d\n", &VbeInfoStructure);
   //kprintf("vbe addr: %d\n", &VbeModeInfoStructure);
   //kprintf("vbe: %i\n", VbeInfoStructure.video_modes[0]);
   //kprintf("vbe2: %i\n", VbeInfoStructure.video_modes[1]);
   // *(tempMem) = VbeModeInfoStructure.framebuffer;
  // mapPage(0x0, (uint32_t*)VbeModeInfoStructure.framebuffer, 0x0);

   //uint16_t* modes = (uint16_t* )(VbeInfoStructure.video_modes[0] * 0x10 + VbeInfoStructure.video_modes[1]);
   /*uint16_t* modes = (uint16_t *)VbeInfoStructure.video_modes[0];
   uint8_t keepGoing = 1;
   uint8_t i = 0;
   while(keepGoing){
      kprintf("mode: %i\n", modes[i]);
      if (modes[i] = 980) {
         keepGoing = 0;
         kprintf("Found! %i\n", i);
         break;
      }
      if (modes[i] = 65535) {
         kprintf("not found\n");
         break;
         keepGoing = 0;
      }
      i++;
   }*/
//   kprintf("modeArray: %d\n", modes);
//   kprintf("modebuffer: %i\n", vga_mem);


   drawRect(0, 0, 1920, 100, 55);
/*
   for (uint16_t i = 0; i < VGA_WIDTH; i++) {
      for (uint16_t j = 0; j < VGA_HEIGHT; j++) {

         //this one works
         uint8_t* pixel = VGA_MEM + j * VGA_WIDTH + i;
         *pixel = 55;
         // *(vga_mem + j * VGA_WIDTH + i) = 15;
         // *((uint8_t *)0xa0000 + j * VGA_WIDTH + i) = 15;
         //putpixel(VGA_MEM, i, j, 15);

      }
   
   }

   for(uint32_t k = 0x0FC000; k > 0xa0000; k--){

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
