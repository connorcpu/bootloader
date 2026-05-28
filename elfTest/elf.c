#include "../stdlib/syscalls.h"
#include "../stdlib/stdio.h"


void drawRect(uint8_t _rgb[], uint8_t* loc);
void repos();

uint8_t fd;
//uint8_t buffer[1080][1920*3];
//uint8_t buffer[6220800];
uint8_t buffer[576000];
uint8_t* loc = buffer;

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
   //drawRect(rgb);

   //uint8_t* loc = buffer+1920*3*10;
   //uint8_t* loc = buffer;

   for(uint8_t i = 0; i < 500; i++){

      repos();

      for(uint8_t j = 0; j < 255; j++){
         rgb[1] = j;
         drawRect(rgb, loc);
      }

      for(uint8_t k = 255; k > 0; k--){
         rgb[0] = k;
         drawRect(rgb, loc);
      }

      for(uint8_t l = 0; l < 255; l++){
         rgb[2] = l;
         drawRect(rgb, loc);
      }

      for(uint8_t m = 255; m > 0; m--){
         rgb[1] = m;
         drawRect(rgb, loc);
      }

      for(uint8_t n = 0; n < 255; n++){
         rgb[0] = n;
         drawRect(rgb, loc);
      }

      for(uint8_t o = 255; o > 0; o--){
         rgb[2] = o;
         drawRect(rgb, loc);
      }
   
      //if(i >= 4) i = 0;

   }

   return;


}

void drawRect(uint8_t _rgb[], uint8_t* where){

   //uint8_t* volatile vga_mem = (uint8_t *)0x2000000;
   //uint8_t* where = buffer;
   repos();

   for(uint8_t j = 0; j < 10; j++){

      for (uint8_t k = 0; k < 10; k++) {

         where[k*3] = _rgb[0];
         where[k*3 + 1] = _rgb[1];
         where[k*3 + 2] = _rgb[2];
      
      }
         where += (1920*3);

   }

   write(fd, buffer, (1920*3*100));

}

void repos(){

      uint8_t scancode = poll(0,0,0);
      if(scancode == 0xff){ }
      else if(scancode == 0x1e){ loc -= 10*3; }
      else if(scancode == 0x20){ loc += 10*3; }
      else if(scancode == 0x1f){ loc += 1920*3*10; }
      else if(scancode == 0x11){ loc -= 1920*3*10; }

}

