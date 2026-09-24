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
         case 'f':
            double tmp = va_arg(args, double);
            if(tmp < 0.0f){buf[0] = '-'; f2a(-tmp, buf + 1);}else{
            f2a(va_arg(args, double), buf);}
            print(buf);
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

char ptr[255];

char* readLine(uint8_t fd){
   for(uint8_t i = 0; i < 255; i++){ ptr[i] = 0x00;}

   uint8_t i = 0;
   do{
      if(ptr[i] == 'EOF')return ptr;
      read(fd, ptr + i, 1);
     // kprintf("%c", ptr[i]);
      i++;
   } while(ptr[i-1] != '\n');

   return ptr;

}

void f2a(float x, char* p)
{
  int n,i=0,k=0;
  n=(int)x;
  while(n>0)
  {
    x/=10;
    n=(int)x;
    i++;
 }
 if(i == 0){*p = 0; i++;}
 *(p+i) = '.';
 x *= 10;
 n = (int)x;
 x = x-n;
 while((n>0)||(i>k))
 {
   if(k == i)
        k++;
   *(p+k)='0'+n;
   x *= 10;
   n = (int)x;
   x = x-n;
   k++;
 }
 /* Null-terminated string */
 *(p+k) = '\0';
}

char k2a(uint8_t keycode){

   switch(keycode){

      case 0x1e:
         return 'a';
      case 0x30:
         return 'b';
      case 0x2e:
         return 'c';
      case 0x20:
         return 'd';
      case 0x12:
         return 'e';
      case 0x21:
         return 'f';
      case 0x22:
         return 'g';
      case 0x23:
         return 'h';
      case 0x17:
         return 'i';
      case 0x24:
         return 'j';
      case 0x25:
         return 'k';
      case 0x26:
         return 'l';
      case 0x32:
         return 'm';
      case 0x31:
         return 'n';
      case 0x18:
         return 'o';
      case 0x19:
         return 'p';
      case 0x10:
         return 'q';
      case 0x13:
         return 'r';
      case 0x1f:
         return 's';
      case 0x14:
         return 't';
      case 0x16:
         return 'u';
      case 0x2f:
         return 'v';
      case 0x11:
         return 'w';
      case 0x2d:
         return 'x';
      case 0x15:
         return 'y';
      case 0x2c:
         return 'z';
      default:
         return ' ';

   }

}
