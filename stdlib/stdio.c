#include "stdio.h"
#include <stdint.h>
#include "string.h"
#include "syscalls.h"

unsigned char* vidmem;

/*int ioInit(){

   vidmem = (unsigned char *) VIDEO_ADDRESS;

}*/

//int putch(char character, uint8_t x, uint8_t y){
int putch(char character){

//   vidmem[(y * 160) + (x * 2)] = character;
//   writeSerial(character);
      
   write(1, &character, 1);

}

int print(char* string){

   /*for(int i = 0; string[i] != 0; i++){
      printch(string[i]);
   }*/
   
   uint16_t len = 0;
   uint8_t* ptr = string;
   while(*ptr != '\0'){
      ptr++;
      len++;
   }

   write(1, string, len);

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
         
         //it seems that d produces cononical addresses while i produceses linear
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

   if(decn == 0){
      kprintf("0x0");
      return ;
   }

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
      //writeSerial('\n');
      write(1, "\n", 1);
      //writeSerial('\r');
      return;
   }
   //putch(ch, x, lineNum);
   putch(ch);

   return;
}

