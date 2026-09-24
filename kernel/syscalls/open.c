#include "open.h"
#include "../io.h"
#include "../syscall.h"
#include "../fat.h"
#include "../utils.h"
#include "../vfs.h"
#include "fileDescriptor.h"

uint8_t sysOpen(char* fileName, uint32_t flags, uint8_t mode){

   kprintf("tried to open file %s in mode %h\n", fileName, mode);

   return open(fileName, flags, mode);


   return -1;

}
