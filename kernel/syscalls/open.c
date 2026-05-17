#include "open.h"
#include "../io.h"
#include "../syscall.h"
#include "fileDescriptor.h"

uint8_t sysOpen(char* fileName, uint32_t flags, uint8_t mode){

   kprintf("tried to open file %s in mode %h\n", fileName, mode);

   if(fileName == "/dev/fb"){

      //VbeModeInfoStructure_t* VBEIBA = getVBEIBA();
      //return registerfd(VBEIBA.framebuffer);
      return 0x2000000;

   }

   return -1;

}
