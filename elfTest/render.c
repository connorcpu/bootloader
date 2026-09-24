#include "../stdlib/syscalls.h"
#include "../stdlib/stdio.h"
#include "../stdlib/math.h"
#include "../stdlib/graphics.h"

float points[24];
uint8_t buffer[6220800];
int64_t translate[3];
uint64_t scale = 100;
char buff[256];
int64_t projected[16];
int64_t transformed[24];
uint8_t edges[24];

bool stop = false;
uint8_t fd;
pollfd_t pfd;


typedef struct object{

   float* points;
   uint16_t* faces;
   uint16_t pointCount;
   uint16_t facecount;
   float translate[3];
   float rotate[4];
   uint8_t fd;
   

} object_t;

object_t objects[1];

#define FOV 2

void draw();
//void putPixel(int64_t x, int64_t y);
void checkInput();
//void drawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2);
int64_t* applyTransform(float _points[]);
int64_t* projectPoints(int64_t _points[]);

uint8_t color[3];
uint8_t keyfd;

void _start(){

   object_t cube;
   uint8_t cubefd = open("/objects/cube.obj", 0x0, 0x0);
   cube.fd = cubefd;
   kprintf("fd: %d\n", cubefd);
   cube.points = (float*)&points;
   objects[0] = cube;

   keyfd = open("/dev/keyboard", 0x0, 0x0);


   color[0] = 255;
   color[1] = 0;
   color[2] = 255;

   kprintf("size: %d\n", sizeof(objects));
   for(uint8_t o = 0; o < sizeof(objects)/sizeof(object_t); o++){

      char* lineBuf = readLine(objects[o].fd);
      //kprintf("linebuf: %s\n", lineBuf);
   
      for(uint8_t i = 0; i < 8; i++){
         
         char* lbuf = readLine(objects[o].fd);
         char floatbuff[256] = {0};

         if(lbuf[0] == 'v' && lbuf[1] == ' '){

            float tmp = parseFloat(lbuf + 2);
            objects[o].points[(i*3)] = tmp;
            uint8_t k = 1;

            for(uint8_t j = 2; j < 255 && k < 3; j++){

               if(lbuf[j] == ' '){

                  objects[o].points[(i*3)+k] = parseFloat(lbuf + j + 1); 
                  k++;

               }
            }
         }
      }
   }

   edges[0] = 0; //back plane
   edges[1] = 1;
   edges[2] = 0;
   edges[3] = 3;
   edges[4] = 1;
   edges[5] = 2;
   edges[6] = 2;
   edges[7] = 3;

   edges[8] = 4; //front plane
   edges[9] = 5;
   edges[10] = 4;
   edges[11] = 7;
   edges[12] = 5;
   edges[13] = 6;
   edges[14] = 6;
   edges[15] = 7;
   
   edges[16] = 0; //connection
   edges[17] = 4;
   edges[18] = 1;
   edges[19] = 5;
   edges[20] = 2;
   edges[21] = 6;
   edges[22] = 3;
   edges[23] = 7;

   kprintf("starting renderer\n");

   fd = open("/dev/fb", 0x0, 0x0);
   kprintf("recieved: %d\n", fd);

   pfd.fd = 0;
   pfd.events = 0x001;

   translate[2] = 2;

   while(!stop){
      checkInput();
      draw(); 
   }

   exit(0);
   

}

void checkInput(){

   //if(poll(&pfd, 1, 0)){
      if(read(keyfd, (uint8_t*)buff, 1) != 0){
   
         switch(buff[0]){
            
            case 75:
               //left
               translate[0] -= 30;
               break;
            case 80:
               //down
               translate[1] += 30;
               break;
            case 77:
               //right
               translate[0] += 30;
               break;
            case 72:
               //up
               translate[1] -= 30;
               break;

         }

      }
   //}

}

uint8_t black[3] = {0};

//pipeline should be:
//(0. apply scale)
//1. apply pos & rot transform
//2. project
//3. draw polygons

void draw(){

   drawRect(black, 0, 1920, 1080, buffer);

   int64_t prevX = 0;
   int64_t prevY = 0;

   applyTransform(points);
   projectPoints(transformed);
   
   for(uint8_t i = 0; i < 16; i += 2){
      putPixel(projected[i], projected[i+1], color, buffer);
   }

   for(uint8_t i = 0; i < 24; i += 2){
      uint64_t x1 = projected[edges[i]*2];
      uint64_t y1 = projected[edges[i]*2 + 1];
      uint64_t x2 = projected[edges[i+1]*2];
      uint64_t y2 = projected[edges[i+1]*2 + 1];
      drawLine(x1, y1, x2, y2, color, buffer);
   }

   write(fd, buffer, (1920*3*1080));

}

int64_t* applyTransform(float _points[]){

   for(uint8_t i = 0; i < 24; i += 3){

      //x = x / (1 + (z / fov));
      //y = y / (1 + (z / fov));
      int64_t x0 = (points[i] * scale) + translate[0];
      int64_t y0 = (points[i+1] * scale) + translate[1];
      int64_t z0 = (points[i+2] ) + translate[2];

      transformed[i] = x0;
      transformed[i+1] = y0;
      transformed[i+2] = z0;

   }

   return transformed;

}

int64_t* projectPoints(int64_t _points[]){

   //int64_t[] projected = (int64_t*)kmalloc((sizeof(_points) / 3 ) * 2);
   uint8_t j = 0;
   for(uint8_t i = 0; i < 24; i += 3){
      projected[j] = _points[i] / (1 + (_points[i+2] / FOV));
      projected[j+1] = _points[i+1] / (1 + (_points[i+2] / FOV));
      j += 2;
   }

   return projected;

}

