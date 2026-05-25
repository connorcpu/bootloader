#include "open.h"
#include "../io.h"
#include "../syscall.h"
#include "../fat.h"
#include "../utils.h"
#include "fileDescriptor.h"

uint8_t sysOpen(char* fileName, uint32_t flags, uint8_t mode){

   kprintf("tried to open file %s in mode %h\n", fileName, mode);

   if(strcmp(fileName, "/dev/fb") == 0){

      //VbeModeInfoStructure_t* VBEIBA = (void*)getVBEIBA();
      //return registerfd((void*)VBEIBA->framebuffer);
      return registerfd((void*)0x2000000);
      //return 0x2000000;

   }

   return -1;

}
