#include <stdint.h>
#include "read.h"
#include "../syscall.h"
#include "../io.h"
#include "../vfs.h"
#include "../memory.h"
#include "../debug.h"

size_t sysRead(uint64_t rdi, uint64_t rsi, uint64_t rdx){

   //kprintf("sys: attempting read, fd: %d\n", rdi);
//    fileDesc_t* fd = retrievefd(rdi);
   vfsfile_t* file = retrievefd(rdi);
   if(!file->inUse) kprintf("not in use\n");
   if(!file->inUse) return -1;

   return file->fops->read(file, (void*)rsi, rdx);
   //memcpy((void*)rsi, file.disk->loc + file.disk->seekLoc, rdx);
   //file.disk->seekLoc += rdx;

   
   return 1;

}

