#include "io.h"

uint8_t lineNum;
unsigned char* vidmem;

int ioInit(){

   vidmem = (unsigned char *) VIDEO_ADDRESS;
   lineNum = 21;

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
   lineNum++;

}
