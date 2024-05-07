#include "io.h"
#include <stdint.h>
#include "utils.h"

uint8_t lineNum;
uint8_t x;
unsigned char* vidmem;
bool shift = false;

int ioInit(){

   vidmem = (unsigned char *) VIDEO_ADDRESS;
   lineNum = 21;
   x = 0;

}

int putch(char character, uint8_t x, uint8_t y){

   vidmem[(y * 160) + (x * 2)] = character;

}

int putst(char* string, uint8_t y){

   for (int i = 0; string[i] != '\0'; i++) {

      putch(string[i], i, y);

   }

}

int print(char* string){

   putst(string, lineNum);
   if (lineNum > 24) {
      lineNum = 0; 
   }else {
      lineNum++;
   }
   
   x = 0;
   movCursor(x, lineNum);
}

void printch(char ch){

   if(shift) ch -= ' ';
   putch(ch, x, lineNum);
   if(x >= 79){
      x = 0;
      lineNum++;
   }else {
      x++;
   }

   movCursor(x, lineNum);
}

void backspace(){

   if(x == 0){
      lineNum--;
      x = 79;
   }else{
      x--;
   }

   putch(0, x, lineNum);

   movCursor(x, lineNum);
}

void enter(){

   x = 0;
   lineNum++;
   movCursor(x, lineNum);

}

void cls(){

   for(int i = 0; i < 26; i++){

      for(int j = 0; j < 80; j++){

         putch(0, j, i);

      }

   }

   x = 0;
   lineNum = 0;
   movCursor(0, 0);

}

void setShift(bool enable){

  shift = enable; 

}

void movCursor(uint8_t x, uint8_t y){

   uint16_t pos = y * 80 + x;
   outb(0x3D4, 0x0f);
   outb(0x3D5, (uint8_t) (pos & 0xFF));
   outb(0x3D4, 0x0E);
   outb(0x3D5, (uint8_t) ((pos >> 8) && 0xFF));

}
