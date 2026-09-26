#include <stdint.h>

enum fileType {
   device, file, memory
};

#define RINGSIZE 255
typedef struct vfsfile vfsfile_t;

typedef struct fileops{

   //file struct, buffer pointer, count to be read/wrote
   uint64_t (*read)(vfsfile_t*, void*, uint64_t);
   uint64_t (*write)(vfsfile_t*, void*, uint64_t);

   uint8_t (*open)(vfsfile_t*);
   uint8_t (*close)(vfsfile_t*);

   uint8_t (*ioctl)(vfsfile_t*, uint64_t, void*);

   //uint8_t (*poll)(struct vfsfile_t*);
   //uint8_t (*mmap)(struct vfsfile_t*, ...);


}fileops_t;

typedef struct device{

   void* ringHead;
   void* ringTail;
   void* ringAllocStart;
   void* readQueue;

}device_t;

typedef struct diskFile{

   void* loc;
   uint64_t seekLoc;
   uint64_t size;

}diskFile_t;

typedef struct vfsfile{
   
   uint8_t inUse : 1;
   enum fileType type;
   fileops_t* fops;
   device_t* dev;
   diskFile_t* disk;

}vfsfile_t;

void ringAppend(vfsfile_t* file, char* buff, uint8_t size);
void ringConsume(vfsfile_t* file, char* buff, uint8_t size);
uint64_t readFromDisk(vfsfile_t* file, void* buff, uint64_t count);
uint64_t writeToDisk(vfsfile_t* file, void* buff, uint64_t count);
uint64_t readFromZero(vfsfile_t* file, void* buff, uint64_t count);
uint8_t findEmptyfd();
uint8_t registerDiskFilefd(char* fileName);
uint8_t open(char* fileName, uint32_t flags, uint8_t mode);
uint8_t devOpen(vfsfile_t* file);
vfsfile_t* retrievefd(uint8_t fd);
void init();
uint64_t fbWrite(vfsfile_t* file, void* buff, uint64_t count);
uint64_t keyRead(vfsfile_t* file, void* buff, uint64_t count);
void handleKeys();
uint64_t ttyWrite(vfsfile_t* file, void* buff, uint64_t count);
uint8_t ttyioctl(vfsfile_t* file, uint64_t cmd, void* arg);
