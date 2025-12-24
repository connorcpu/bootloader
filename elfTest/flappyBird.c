#include <stdint.h>
#include "../stdlib/stdGraphics.h"

#define VGA_WIDTH 1920
#define VGA_HEIGHT 1080

const int playerSize = 20;
const int playerX = 20;
int playerY = VGA_HEIGHT / 2 - playerSize / 2;

int _start()
{

   drawRect(0, 0, VGA_WIDTH, VGA_HEIGHT, 55);

   return 0;
}
