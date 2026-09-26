#include "ioctl.h"
#include "../vfs.h"
#include "../io.h"

uint8_t sysIoctl(uint64_t rdi, uint64_t rsi, uint64_t rdx){

   vfsfile_t* file = retrievefd(rdi);
   if(!file->inUse) {kprintf("not in use\n"); return -1;}

   return file->fops->ioctl(file, (uint64_t)rsi, (void*)rdx); 

}
