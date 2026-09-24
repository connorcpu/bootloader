//#include "vfs.h"
#include "fat.h"
#include "font.h"
#include "io.h"

uint8_t width;
uint8_t height;
uint8_t byteSize;
PSF_font* file;
uint8_t widthBytes;

void initFont(){
   
    //file = (PSF_font*)loadFile("/etc/fonts/autism.psf");
    file = (PSF_font*)loadFile("/etc/fonts/alt.psf");
    kprintf("magic: %h\nversion: %d\nheadersize: %d\nflags: %h\nnumglyph: %d\nbytesperglyph: %h\nheight: %d\nwidth: %d\n", file->magic, file->version, file->headersize, file->flags, file->numglyph, file->bytesperglyph, file->height, file->width);

    width = file->width;
    height = file->height;
    byteSize = file->bytesperglyph;

    if(file->flags != 0x1) kprintf("WARNING: font non unicode mapped, this will probably cause problems");

    if(width <= 8){widthBytes = 1;}
    else if(width <= 16){widthBytes = 2;}
    else if(width <= 24){widthBytes = 3;}
    else if(width <= 32){widthBytes = 4;}
    //widthBytes = (width + 7) / 8; //might work might not

    kprintf("widthbytes: %d\n", widthBytes);

    for(uint8_t i = 0; i < 26; i++){
       //renderChar('a' + i, i, 0);
    }


}

void rendChar(char c, uint32_t offset){

   renderChar(c, offset % (1920/width), offset / (1920 / width));

}

void renderChar(char c, uint16_t cx, uint16_t cy){

   //we run 1920x1080
   //width is 10, height is 20.
   //therfore theres 54 height slots, 0-53
   //and 192 width slots, 0-191
   
   if(cx >= (1920/width) || cy >= (1080/height) || cx < 0 || cy < 0) return;

   uint8_t* loc = (void*)file + file->headersize;
   uint8_t fg[3] = {255, 255, 255};
   uint8_t bg[3] = {0, 0, 0};

   //kprintf("file + header: %h\n", loc);

   //char as number is the index since it uses a unicode table
   uint8_t* charLoc = loc + (file->bytesperglyph * (uint8_t)c);

   //kprintf("char will be at: %h\n", charLoc);

   uint8_t* where = (void*)0x2000000 + (cx * width * 3) + (cy * height * 3 * 1920);

   //vertical loop
   for(uint8_t y = 0; y < height; y++){

      uint8_t mask = 1<<7;
      uint8_t* currentByte = charLoc + (y * widthBytes);
      for(uint8_t x = 0; x < width; x++){

         *(where + (x*3) + (y*3*1920) + 0) = (*currentByte & mask) ? fg[0] : bg[0];
         *(where + (x*3) + (y*3*1920) + 1) = (*currentByte & mask) ? fg[1] : bg[1];
         *(where + (x*3) + (y*3*1920) + 2) = (*currentByte & mask) ? fg[2] : bg[2];
         mask >>= 1;

         if(mask == 0){
            mask = 1<<7;
            currentByte += 1;
         }




      }


   }


}
