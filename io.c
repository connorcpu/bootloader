#include "io.h"
#include "debug.h"
#include <stdint.h>

uint8_t lineNum;
uint8_t x;
unsigned char* vidmem;
bool shift, alt, ctrl = false;

void setmod(uint8_t modkeyindx, bool enabled){

   //1 = shift; 2 = alt; 3 = ctrl 
   switch (modkeyindx) {
      case 1:
         shift = enabled;
         break;
      case 2:
         alt = enabled;
         break;
      case 3:
         ctrl = enabled;
         break;
   
   }

}

int ioInit(){

   vidmem = (unsigned char *) VIDEO_ADDRESS;
   lineNum = 21;
   x = 0;

}

int putch(char character, uint8_t x, uint8_t y){

   vidmem[(y * 160) + (x * 2)] = character;
   writeSerial(character);

}

int putst(char* string, uint8_t x, uint8_t y){

   for (int i = 0; string[i] != 0; i++) {

      putch(string[i], x, y);
      if(x >= 79){
         x = 0;
         lineNum++;
      }else {
         x++;
      }

   }

}

int print(char* string){

   for(int i = 0; string[i] != 0; i++){
      printch(string[i]);
   }
   if (lineNum > 24) {
      lineNum = 0; 
   }else {
     // lineNum++;
   }
   
   x = 0;
   movCursor(x, lineNum);
}

void kprintf(char * string, ...){

   va_list args;
   va_start(args, string);
   format(string, args);
   va_end(args);

}

void format(char *string, va_list args){
   
   char buf[48] = {0};
   char ch; 

   while((ch = *(string++))){

      if(ch != '%') printch(ch);
      if(ch == '%'){

         ch = *(string++);
         
         switch (ch) {
         case 0: return;
         case 'c':
            printch(va_arg(args, int));
            break;
         case 's':
            print(va_arg(args, char*));
            break;
         case 'd':
            i2a(va_arg(args, long int), buf);
            print(buf); 
            break;
         case 'i':
            i2a(va_arg(args, int), buf);
            print(buf);
            break;
         case 'h':
            i2h(va_arg(args, uint64_t), buf);
            break;
         case '%':
            printch(ch);
            break;
         
         }

      }
   
      for(uint8_t i = 0; i < 48; i++){

         buf[i] = 0;

      }
   }

   return;

}

void i2h(uint64_t decn, char* buffer){

   uint64_t q;
   uint64_t m;

   uint32_t indx = 0;
   uint64_t tmp;

   while(decn > 0){

      tmp = decn % 16;

      if(tmp < 10){
         buffer[indx] = tmp + '0';
      }else{
         buffer[indx] = tmp + 'A' - 10;
      }
      indx++;
      decn /= 16;

   }
   kprintf("0x");

   for(int i = indx - 1; i >= 0; i--){
      //printch(buffer[i]);
      kprintf("%c", buffer[i]);
   }

}

void i2a(uint64_t num, char* buffer){

   uint64_t n = 0;
   uint64_t dgt;
   uint64_t d = 1;

   while((num / d) >= 10) d *= 10; 
   while(d != 0){

      dgt = num / d;
      num %= d; 
      d /= 10;
      if (n || dgt > 0 || d == 0) {
         *buffer++ = dgt + (dgt < 10 ? '0' : 'a' - 10);
         ++n;
      
      }

   }
   *buffer == 0;

}

void printch(char ch){

   if(ch == '\n'){
      enter();
      writeSerial('\n');
      writeSerial('\r');
      return;
   }
   if(shift) ch -= ' ';
   putch(ch, x, lineNum);
   if(x >= 79){
      x = 0;
      lineNum++;
   }else {
      x++;
   }

   movCursor(x, lineNum);

   return;
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

   return;

}

void enter(){

   x = 0;
   lineNum++;
   movCursor(x, lineNum);

   return;

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
   outb(0x3D4, 0x0F);
   outb(0x3D5, (uint8_t) (pos & 0xFF));
   outb(0x3D4, 0x0E);
   outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));

}
