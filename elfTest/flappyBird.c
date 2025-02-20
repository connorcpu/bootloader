#include <stdint.h>

#define VGA_WIDTH 300
#define VGA_HEIGHT 200
#define VGA_MEM (uint8_t *) 0xa0000

const int playerSize = 20;
const int playerX = 20;
int playerY = VGA_HEIGHT / 2 - playerSize / 2;

void drawRect(int x, int y, int width, int height, int colour){

   for(int i = x; i < width; i++){
      for (int j = y; j < height; j++) {

         *(VGA_MEM + j * VGA_WIDTH + i) = colour;
      
      }

   }

}

int main()
{
   //drawRect(0, 0, VGA_WIDTH, VGA_HEIGHT, 15);
   //drawRect(playerX, playerY, playerSize, playerSize, 55);

   for (uint32_t i = 0; i < VGA_WIDTH; i++) {
      for (uint32_t j = 0; j < VGA_HEIGHT; j++) {

         *((uint8_t *) 0xa0000 + j * VGA_WIDTH + i) = 15;
      
      }
   
   }

   return 0;
}
