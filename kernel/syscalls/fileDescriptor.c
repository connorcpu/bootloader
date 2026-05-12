#include "fileDescriptor.h"
#include "../io.h"
#include <stdint.h>

//a array storing void pointers so 64 bit entries
void* table[256];

void startfd(){

   //just zero'ing the array so we can rely on the fact that zero means unassigned
   for(uint8_t i = 2; i < 255; i++){
      table[i] = 0;
   }

   //just say no to this first 3 descriptors
   table[0] = (void*)-1;
   table[1] = (void*)-1;
   table[2] = (void*)-1;

}


//the idea is this thing knows can allocate file descriptors when files or sockets are opened, 
//their corrisponding pointers (might have to be changed out for structs to support things like sockets) are stored in a table
//the file descriptors can then be requested by another method for things like read/write
uint8_t registerfd(void* fileLoc){

   for(uint8_t i = 0; i < 256; i++){
      if(table[i] == 0){
         table[i] = fileLoc;
         return i;
      }   
      if(i == 256) return -1;
   } 

}

//this function should retrieve the pointer (or in the future structs) from the array
void* retrievefd(uint8_t fd){

   return table[fd];

}
