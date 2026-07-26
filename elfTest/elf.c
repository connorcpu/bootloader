#include "../stdlib/syscalls.h"
#include "../stdlib/stdio.h"

#define POLLIN 0x001


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

pollfd_t pfd;
char buff[256];

void _start(){

   char c = 'C';
   char* testStr = "substitution";
   kprintf("hello from %c code, now with %s\n", c, testStr);
   fd = open("/dev/fb", 0x0, 0x0);

   kprintf("recieved fd: %d\n", fd);

   pfd.fd = 0;
   pfd.events = POLLIN;

   for(uint8_t i = 0; i < 255; i++){
      buff[i] = 0;
   }

   playerColour[0] = 255;
   playerColour[1] = 0;
   playerColour[2] = 0;


   while(true){
   
      draw();
      if(lost){
         exit(0);
      }

   }

   exit(0);
   return;


}

uint8_t black[3];
uint8_t red[3];

void checkCollision(){

   //874 works, 873 does not
   if(playerHeight + velocity < 0 || playerHeight + velocity + 50 >= 1080){
      
      kprintf("you went out of bounds, %i, %i\n", playerHeight, velocity);
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
   //if(poll(0,0,0) == 0x39) {
   
   char result = -1;

   //array of pollfd_t's, int of how many fd's. optional timeout
   if(poll(&pfd, 1, 0)){

      //fd, buffer, size
      if(read(0, buff, 1) != 0){
         
         if(buff[0] == 57){
            velocity = -10;
         }else{
            velocity += 1;
         }

      }
   for(uint8_t i = 0; i < 255; i++){
      buff[i] = 0;
   }
      

   }

   /*if(){
      velocity = -10;
   }else{
      velocity += 1;
   }*/

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

