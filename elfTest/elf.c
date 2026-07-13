#include "../stdlib/syscalls.h"
#include "../stdlib/stdio.h"


void drawRect(uint8_t _rgb[], uint8_t* loc, uint16_t width, uint16_t height);
void repos();
void draw();

uint8_t fd;
uint8_t buffer[6220800]; //1920*1080*3

uint8_t playerColour[3];
uint16_t playerHeight = 100;
int16_t velocity = 0;

uint16_t blockHeight = 1080/2;
uint16_t blockX = 1820;
uint8_t blockVel = 10;
uint16_t blockGap = 200;

bool lost = false;

void _start(){

   char c = 'C';
   char* testStr = "substitution";
   kprintf("hello from %c code, now with %s\n", c, testStr);
   fd = open("/dev/fb", 0x0, 0x0);

   //var substition currently NOT working
   kprintf("recieved fd: %d\n", fd);
   kprintf("local variable allocated at: %h\n", &c);
   /*uint8_t rgb[3];
   rgb[0] = 255;
   rgb[1] = 0;
   rgb[2] = 0;*/
   playerColour[0] = 255;
   playerColour[1] = 0;
   playerColour[2] = 0;

   //drawRect(rgb);

   //uint8_t* loc = buffer+1920*3*10;
   //uint8_t* loc = buffer;

   for(uint8_t i = 0; i < 500; i++){

      //repos();
      draw();
      if(lost) {
         exit(0);
      }

      /*for(uint8_t j = 0; j < 255; j++){
         playerColour[1] = j;
         draw();
      }

      for(uint8_t k = 255; k > 0; k--){
         playerColour[0] = k;
         draw();
      }

      for(uint8_t l = 0; l < 255; l++){
         playerColour[2] = l;
         draw();
      }

      for(uint8_t m = 255; m > 0; m--){
         playerColour[1] = m;
         draw();
      }

      for(uint8_t n = 0; n < 255; n++){
         playerColour[0] = n;
         draw();
      }

      for(uint8_t o = 255; o > 0; o--){
         playerColour[2] = o;
         draw();
      }*/
   
      //if(i >= 4) i = 0;

   }

   exit(0);
   return;


}

uint8_t black[3];
uint8_t red[3];

void checkCollision(){

   if(playerHeight + velocity < 0 || playerHeight + velocity + 50 >= 1920){
      
      kprintf("you went out of bounds\n");
      lost = true;

   }

   //first part is to check height
   //second part is to check x
   if((playerHeight < blockHeight || playerHeight + 50 > blockHeight + blockGap) && (blockX <= 100)) lost = true;

   return;

}

void draw(){
   
   black[0] = 0;
   black[1] = 0;
   black[2] = 0;

   red[0] = 0;
   red[1] = 0;
   red[2] = 255;

   drawRect(black, (uint8_t*)0, 1920, 1080); //this one is faulty
   //drawRect(playerColour, (uint8_t*)(1920*3*playerHeight), 50, 50);
   if(poll(0,0,0) == 0x39) {
      velocity = -10;
   }else{
      velocity += 1;
   }

   if(blockX - blockVel <= 0) {
      blockX = 1820;
   }else{
      blockX -= blockVel;
   }

   //draw blocks
   drawRect(red, (uint8_t*) (blockX * 3), 100, blockHeight);
   drawRect(red, (uint8_t*) ((blockX * 3) + (1920*3*(blockHeight + blockGap))), 100, (1080-(blockHeight + blockGap)));

   checkCollision();
   if(lost) return;

   playerHeight += velocity;


   drawRect(playerColour, (uint8_t*)(1920*3*playerHeight + (50*3)), 70, 50);
   write(fd, buffer, (1920*3*1080));
}


void drawRect(uint8_t _rgb[], uint8_t* where, uint16_t width, uint16_t height){

   //uint8_t* volatile vga_mem = (uint8_t *)0x2000000;
   //uint8_t* where = buffer;
   //repos();

   where += (uint64_t)buffer;

   for(uint16_t j = 0; j < height; j++){

      for (uint16_t k = 0; k < width; k++) {
         
         if(where < buffer || _rgb < buffer){ kprintf("bruh: %h\n", _rgb);}//__asm__ volatile("xchg %bx, %bx");}
         //the existance of the above line is the sole fix for a totally unrelated bug, probably something with the stack.
         //please try comparing the different compilations

         where[k*3] = _rgb[0];
         where[k*3 + 1] = _rgb[1]; //this line causing issues
         where[k*3 + 2] = _rgb[2];
      
      }
         where += (1920*3);

   }
   //kprintf("where: %h\n", where);

   //write(fd, buffer, (1920*3*1080));

}

/*void repos(){

      uint8_t scancode = poll(0,0,0);
      if(scancode == 0xff){ }
      else if(scancode == 0x1e){ loc -= 10*3; }
      else if(scancode == 0x20){ loc += 10*3; }
      else if(scancode == 0x1f){ loc += 1920*3*10; }
      else if(scancode == 0x11){ loc -= 1920*3*10; }

}*/

