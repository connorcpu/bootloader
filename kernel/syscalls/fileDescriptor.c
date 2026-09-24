#include "fileDescriptor.h"
#include "../io.h"
#include <stdint.h>

//a array storing void pointers so 64 bit entries
//void* table[256];
fileDesc_t table[256];

void startfd(){

   //just zero'ing the array so we can rely on the fact that zero means unassigned
   for(uint8_t i = 2; i < 255; i++){
      table[i].loc = 0;
      table[i].seekLoc = 0;
   }

   //just say no to this first 3 descriptors
   table[0].loc = (void*)-1;
   table[1].loc = (void*)-1;
   table[2].loc = (void*)-1;

}


//the idea is this thing knows can allocate file descriptors when files or sockets are opened, 
//their corrisponding pointers (might have to be changed out for structs to support things like sockets) are stored in a table
//the file descriptors can then be requested by another method for things like read/write
uint8_t registerfd(void* fileLoc){

   for(uint8_t i = 0; i < 256; i++){
      if(table[i].loc == 0){
         table[i].loc = fileLoc;
         return i;
      }   
      if(i == 256) return -1;
   } 

}

//this function should retrieve the pointer (or in the future structs) from the array
/*fileDesc_t* retrievefd(uint8_t fd){

   if(table[fd].loc == 0x0){ kprintf("fd not registered\n"); return -1;}
   return table + fd;

}*/
