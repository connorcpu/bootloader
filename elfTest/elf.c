#include "../stdlib/syscalls.h"
#include "../stdlib/stdio.h"


void drawRect(uint8_t _rgb[]);

uint8_t fd;
//uint8_t buffer[1080][1920*3];
//uint8_t buffer[6220800];
uint8_t buffer[576000];

void _start(){

   char c = 'C';
   char* testStr = "substitution";
   kprintf("hello from %c code, now with %s\n", c, testStr);
   fd = open("/dev/fb", 0x0, 0x0);

   //var substition currently NOT working
   kprintf("recieved fd: %d\n", fd);
   uint8_t rgb[3];
   rgb[0] = 255;
   rgb[1] = 0;
   rgb[2] = 0;
   drawRect(rgb);

   for(uint8_t i = 0; i < 50; i++){

      for(uint8_t j = 0; j < 255; j++){
         rgb[1] = j;
         drawRect(rgb);
      }

      for(uint8_t k = 255; k > 0; k--){
         rgb[0] = k;
         drawRect(rgb);
      }

      for(uint8_t l = 0; l < 255; l++){
         rgb[2] = l;
         drawRect(rgb);
      }

      for(uint8_t m = 255; m > 0; m--){
         rgb[1] = m;
         drawRect(rgb);
      }

      for(uint8_t n = 0; n < 255; n++){
         rgb[0] = n;
         drawRect(rgb);
      }

      for(uint8_t o = 255; o > 0; o--){
         rgb[2] = o;
         drawRect(rgb);
      }
   
      //if(i >= 4) i = 0;

   }

   return;


}

void drawRect(uint8_t _rgb[]){

   //uint8_t* volatile vga_mem = (uint8_t *)0x2000000;
   uint8_t* where = buffer;

   for(uint8_t j = 0; j < 100; j++){

      for (uint8_t k = 0; k < 100; k++) {

         where[k*3] = _rgb[0];
         where[k*3 + 1] = _rgb[1];
         where[k*3 + 2] = _rgb[2];
      
      }
         where += (1920*3);

   }

//   write(fd, buffer, (1920*3*100));

}

