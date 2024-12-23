#include <stdint.h>

#define VGA_WIDTH 1280
#define VGA_HEIGHT 1024
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
   drawRect(playerX, playerY, playerSize, playerSize, 55);

   return 0;
}
