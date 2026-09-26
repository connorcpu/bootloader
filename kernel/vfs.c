#include "vfs.h"
#include "io.h"
#include "stdbool.h"
#include "utils.h"
#include "fat.h"
#include "memory.h"
#include "interrupt.h"
#include "font.h"

vfsfile_t files[256];
vfsfile_t keyboard;
vfsfile_t fb;
vfsfile_t tty;

void init(){

   files[0].inUse = true;
   files[1].inUse = true;
   files[2].inUse = true;

   //framebuffer
   fb.inUse = true;
   fb.type = memory;
   fileops_t* fbfo = (fileops_t*)kmalloc(sizeof(fileops_t));
   diskFile_t* fbdf = (diskFile_t*)kmalloc(sizeof(diskFile_t));
   fbdf->loc = (void*)0x2000000;
   fbdf->seekLoc = 0x00;
   fbdf->size = 0x00;
   fb.disk = fbdf;
   fbfo->write = &fbWrite;
   fbfo->open = &devOpen;
   fb.fops = fbfo;


   //keyboard
   keyboard.inUse = true;
   keyboard.type = device;
   fileops_t* keyfo = (fileops_t*)kmalloc(sizeof(fileops_t));
   kprintf("keyfo: %h\n", keyfo);
   //diskFile_t* keydf = (diskFile_t*)kmalloc(sizeof(diskFile_T));
   device_t* keydev = (device_t*)kmalloc(sizeof(device_t));
   keydev->ringHead = kmalloc(256);
   keydev->ringTail = keydev->ringHead;
   keydev->ringAllocStart = keydev->ringHead;
   //keydev->readQueue = ??;
   keyboard.dev = keydev;
   keyfo->read = &keyRead;
   keyfo->open = &devOpen;
   keyboard.fops = keyfo;

   registerInterupt(0x01, &handleKeys);

   tty.inUse = true;
   tty.type = memory;
   fileops_t* ttyfo = (fileops_t*)kmalloc(sizeof(fileops_t));
   diskFile_t* ttydf = (diskFile_t*)kmalloc(sizeof(diskFile_t));
   ttydf->loc = (void*)kmalloc(16200);
   ttydf->seekLoc = 0x00;
   ttydf->size = 16200;
   tty.disk = ttydf;
   ttyfo->write = &ttyWrite;
   ttyfo->open = &devOpen;
   tty.fops = ttyfo;
   ttyfo->ioctl = &ttyioctl;

   //ring test 
   /*device_t* testdev = (device_t*)kmalloc(sizeof(device_t));
   kprintf("testdev: %h, with size %h\n", testdev, sizeof(device_t));
   testdev->ringHead = kmalloc(256);
   testdev->ringTail = testdev->ringHead;
   testdev->ringAllocStart = testdev->ringHead;

   kprintf("ringHead: %h\nringTail: %h\nringAllocStart: %h\n", testdev->ringHead, testdev->ringTail, testdev->ringAllocStart);
   vfsfile_t* test = (vfsfile_t*)kmalloc(sizeof(vfsfile_t));
   test->dev = testdev;
   char* testStr = "Hello World\n\0";
   ringAppend(test, "H", 1);
   ringAppend(test, "E", 1);
   ringAppend(test, testStr, 13);
   kprintf("ringHead: %h\nringTail: %h\nringAllocStart: %h\n", testdev->ringHead, testdev->ringTail, testdev->ringAllocStart);

   char* ret = (char*)kmalloc(15);
   ringConsume(test, ret, 15);
   kprintf("%s\n", ret);
   kprintf("%h, %h\n", *((char*)testdev->ringHead), *((char*)testdev->ringHead + 1));*/

   //init stdin
   //init stdout
   //init stderr
  //init keyboard
  //init null 
  //init zero

}

uint8_t open(char* fileName, uint32_t flags, uint8_t mode){

   kprintf("vfs: accessing %s\n", fileName);

   if(strncmp(fileName, "/dev", 4) == 0){

      kprintf("vfs: opening device %s\n", fileName);

      if(strcmp(fileName, "/dev/fb") == 0){

         //this should be the case for all /dev devices
         return fb.fops->open(&fb);

      }else if(strcmp(fileName, "/dev/keyboard") == 0){
         return keyboard.fops->open(&keyboard);
      }else if(strcmp(fileName, "/dev/tty1") == 0){
         return tty.fops->open(&tty);
      }


   }else{

      kprintf("vfs: opening disk file\n");

      return registerDiskFilefd(fileName);

   }

}

uint8_t devOpen(vfsfile_t* file){

   uint8_t fd = findEmptyfd();
   files[fd] = *file;

   return fd;

}

uint8_t ttyioctl(vfsfile_t* file, uint64_t cmd, void* arg){

   switch(cmd){

      case 0:
         //set
         file->disk->seekLoc = (uint64_t)arg;
         break;
      case 1:
         //mov forward
         file->disk->seekLoc += (uint64_t)arg;
         break;
      case 2:
         //mov backward
         file->disk->seekLoc -= (uint64_t)arg;
         break;
      default:
         kprintf("invalid cmd\n");

   }

}

uint64_t ttyWrite(vfsfile_t* file, void* buff, uint64_t count){

   memcpy(file->disk->loc + file->disk->seekLoc, (void*) buff, count);

   for(uint32_t i = 0; i < count; i++){

      rendChar(*((uint8_t*)file->disk->loc + file->disk->seekLoc + i), file->disk->seekLoc + i);

   }

   file->disk->seekLoc += count;



   return count;

}

void handleKeys(){

   uint8_t sc = inb(0x60);
   //if(sc <= 0x7f)
   ringAppend(&keyboard, &sc, 1);

}

uint64_t keyRead(vfsfile_t* file, void* buff, uint64_t count){

   if(file->dev->ringTail > file->dev->ringHead){
      ringConsume(file, buff, count);
      return count;
   }
   return 0;

}

uint64_t fbWrite(vfsfile_t* file, void* buff, uint64_t count){

   //kprintf("writing to frambuffer\n");

   memcpy(file->disk->loc, (void*)buff, count);

   return count;

}

void ringAppend(vfsfile_t* file, char* buff, uint8_t size){
   
   //kprintf("ringHead: %h\nringTail: %h\nringAllocStart: %h\n", file->dev->ringHead, file->dev->ringTail, file->dev->ringAllocStart);

   if(file->dev->ringTail + size <= file->dev->ringAllocStart + 256){

      //no overflow case
      memcpy(file->dev->ringTail, buff, size);
      file->dev->ringTail += size;

   }else{
      kprintf("failed to append to ringbuffer due to overflow\n");
   }

   return;

}

void ringConsume(vfsfile_t* file, char* buff, uint8_t size){

   if(file->dev->ringHead + size <= file->dev->ringAllocStart + 256){

      //no overflow case 
      //kprintf("consuming from %h, moving into buffer at %h, transfering %d bytes\n", file->dev->ringHead, buff, size);
      memcpy(buff, file->dev->ringHead, size);
      file->dev->ringHead += size;

   }

}

uint64_t readFromDisk(vfsfile_t* file, void* buff, uint64_t count){

   memcpy((void*)buff, file->disk->loc + file->disk->seekLoc, count);
   file->disk->seekLoc += count;

}

uint64_t writeToDisk(vfsfile_t* file, void* buff, uint64_t count){



}

uint8_t ioctlDisk(vfsfile_t* file, uint64_t stuff, void* morestuff){



}

uint8_t closeDisk(vfsfile_t* file){




}

uint64_t readFromZero(vfsfile_t* file, void* buff, uint64_t count){



}

vfsfile_t* retrievefd(uint8_t fd){

   return files + fd;

}

uint8_t findEmptyfd(){

   for(uint8_t i = 0; i < 256; i++){
      if(files[i].inUse == false){

         return i;

      }
      if(i == 256) return -1;
   }

}

uint8_t registerDiskFilefd(char* fileName){

   fileHeader_t hdr = findFile(fileName);
   if(hdr.fileSize == -1){kprintf("failed to open file %s\n", fileName); return -1;}
   void* tmp = loadFile(fileName);
   if(tmp == -1){kprintf("failed to open file %s\n", fileName); return -1;}

   kprintf("vfs: succesfully loading file %s of size %d\n", fileName, hdr.fileSize);

   uint8_t fd = findEmptyfd();
   kprintf("vfs: assigning %d\n", fd);

   vfsfile_t* file = &(files[fd]);
   file->inUse = true;
   file->dev = 0x00;

   fileops_t* ops = (fileops_t*)kmalloc(sizeof(fileops_t));
   diskFile_t* difi = (diskFile_t*)kmalloc(sizeof(diskFile_t));

   ops->read = &readFromDisk;
   ops->write = &writeToDisk;
   ops->ioctl = &ioctlDisk;
   ops->close = &closeDisk;

   file->fops = ops;

   difi->seekLoc = 0x00;
   difi->size = hdr.fileSize;
   difi->loc = tmp;

   file->disk = difi;

   kprintf("vfs: finished, file at %h\n", tmp);
   
   return fd;

}
