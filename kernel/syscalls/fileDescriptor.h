#include <stdint.h>

typedef struct filedesc {

   void* loc;
   uint64_t seekLoc;

} fileDesc_t;

void startfd();
uint8_t registerfd(void* fileLoc);
//fileDesc_t* retrievefd(uint8_t fd);
