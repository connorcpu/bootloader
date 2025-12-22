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
         putpixel((uint16_t*)vga_mem, i, j, colour);

      }

   }

   return;

}

void putpixel(uint16_t* screen, uint16_t x, uint16_t y, uint8_t colour){

   uint16_t* loc = (uint16_t*)vga_mem + (y * VbeModeInfoStructure.width) + x;
   //kprintf("about to segfault?\n");
   //uint8_t* loc = (uint8_t*)0xa0000 + (y * 1920 * 2) + (x * 2); //TODO: VbeModeInfoStructure.framebuffer is not populated correctly
   //kprintf("x: %d, y: %d\n", x, y);
   *loc = colour;
   //kprintf("segfaulted?\n");

}

int initFrame(){

   kprintf("mapping framebuffer\n");
   for(int i = 0; i < 1013; i++){
      mapPage((uint8_t*)VbeModeInfoStructure.framebuffer + (i * 0x1000), (uint8_t*)0x2000000 + (i*0x1000), 0x0); //map "physical" video mem to 4GiB
   }
   kprintf("mapped framebuffer\n");

   vga_mem = (uint16_t *)0x2000000;

   kprintf("framebuffer: %i\n", (uint32_t)VbeModeInfoStructure.framebuffer);
   kprintf("width: %i\n", VbeModeInfoStructure.width);
   kprintf("height: %i\n", VbeModeInfoStructure.height);
   kprintf("bpp: %i\n", VbeModeInfoStructure.bpp); 


   drawRect(0, 0, 1920, 1080, 55);

/*   *((uint8_t*)0xb0000) = 24;
   uint32_t addr = 0xb0000;
   *(uint8_t *)addr = 24;
   *(uint8_t *)(addr + 1) = 24;
*/

   return 0;

}
