#include "fat.h"
#include "io.h"
#include "debug.h"
#include "ide.h"
#include <stdint.h>
#include "utils.h"
#include "memory.h"

fatBootsector_t bootsect;
fatEbr32_t ebrsect;
uint32_t clusterBeginLba;
fileHeader_t rootdir;
fileHeader_t* rootFiles;
uint8_t* pointer;

uint32_t* fat;

void fatInit(){

//   kmalloc(512, (uint32_t*) &pointer);
//
   kprintf("fat: initing fat\n");

   pointer = kmalloc(512);

   fatBootsector_t* bootaddr = (fatBootsector_t*) pointer;
   fatEbr32_t* ebraddr = (fatEbr32_t*) (pointer + sizeof(fatBootsector_t));

   ide_read_sectors(0, 1, 0x000000800, 0x10, (uint64_t)bootaddr); //read VBR

   kprintf("read vbr\n");
   bootsect = *bootaddr;
   ebrsect = *ebraddr;

//   kprintf("fat name (boot sect validation): %s\n", bootsect.oem_name);
   //kprintf("fat name (ebr sect validation): %s\n", ebrsect.volumeLabel);
   fileHeader_t* rootaddr = (fileHeader_t*)kmalloc(bootsect.sectsPerCluster * bootsect.bytesPerSect);
   rootdir = *rootaddr;
   
   clusterBeginLba = 0x000000800 + bootsect.numReservedSects + (bootsect.numFats * ebrsect.fatSize);

   //THIS LINE IS THE ONE ALLOCATING WAAAYYY TO MUCH
   //okay it might be allocating the correct amount
   fat = (uint32_t*) kmalloc(ebrsect.fatSize * bootsect.bytesPerSect);
   //kprintf("fat size: %i\nbytes per sect: %i\n", ebrsect.fatSize, bootsect.bytesPerSect);

   //read the fat 
   //we should not be mapping pages for this
   kprintf("reading fat\n");
   mapPage((uint8_t*)0x1ff0000, (uint8_t*)0x1ff0000, 0x0);
   ide_read_sectors(0, ebrsect.fatSize, 0x800 + bootsect.numReservedSects, 0x10, (uint64_t)fat); 

   kprintf("read fat\n");

   //load root dir
   uint8_t status = ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(ebrsect.rootCluster), 0x10, (uint64_t)rootaddr);
   rootFiles = rootaddr;
   kprintf("loading root dir at %h\n", (uint64_t)rootFiles);
   
   //kprintf("bytes per sect: %d; sects per cluster: %d\n", bootsect.bytesPerSect, bootsect.sectsPerCluster);
   
}

uint8_t* loadFile(char* fileName){

   fileHeader_t file = findFile(fileName);
   
   if(file.fileSize == -1) {
      kprintf("fat: could not find file\n"); 
      return (uint8_t*)-1;
   }

   uint8_t* loc = kmalloc(file.fileSize);

   return (uint8_t*)loadClusterChain(file.startingCluster, (void*)loc);
   //return loc;

}

uint8_t* openFile(char* fileName, fileHeader_t* loadAddr){

   fileHeader_t file = findFile(fileName); 

   kprintf("file size: %d\n", file.fileSize);

   if(file.fileSize == -1) {
      kprintf("fat: could not find file\n"); 
      return (uint8_t*)-1;
   }

   uint8_t* ret = loadClusterChain(file.startingCluster, (void *)loadAddr);
   //kprintf("buh: %h\n", loadAddr);
   return ret;
  // return ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(rootFiles[i].startingCluster), 0x10, (uint32_t)loadAddr);
      
}

fileHeader_t findFile(char* fileName){

   bool found = false;
   fileHeader_t* currentDir = rootFiles;
   uint8_t* toSearch = fileName;
   //uint8_t searching[12] = {0};
   uint8_t* searching = kmalloc(12);
   uint8_t* searchName = fileName;

   for(uint8_t i = 0; i < 12; i++){
      searching[i] = 0x0;
   }

   kprintf("current: %h\n root: %h\n", currentDir, rootFiles);

   while(!found){

      kprintf("searchName: %s\n", searchName);

      //for(uint8_t j = 1; j < len(searchName); j++){
      for(uint8_t i = 0; i < 11; i++){

         if (searchName[i] == '/') {

         
            //searching = (char*)searchName + i + 1;
            uint8_t endingIdx = 0;
            for(uint8_t j = i + 1; j < 15; j++){
               kprintf("%c\n", searchName[j]);
               if (searchName[j] == '/'){
                  endingIdx = j - 1;
                  kprintf("endingIdx: %d\n", endingIdx);
                  break;
               }
               else if (searchName[j] == '\0') {
                  endingIdx = j+ 1;
                  kprintf("yes\n");
                  break;
               }
            }
            
            memcpy(searching, searchName + i + 1, endingIdx - (i));
            
            break;
         }


      }



      kprintf("searching: %s\n", searching);

      uint16_t i = 0;
      while(currentDir[i].name[0] != 0){

         uint8_t name[12] = {0};
         getFileName(&currentDir[i], name);


            kprintf("name: %s\n", name);
         if (strcmp(name, searching) == 0) {
         
            //found the file
//            return loadClusterChain();
            //return ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(rootFiles[i].startingCluster), 0x10, (uint32_t)loadAddr);
            if((currentDir[i].attributes & 0x10) == 0x10){
               //dir case
               kprintf("found\n");
               for(uint8_t i = 1; i < 11; i++){if(searchName[i] == '/'){searchName = searchName + i;}}
               fileHeader_t* tmp = (fileHeader_t*)kmalloc(512);
               loadClusterChain(currentDir[i].startingCluster, tmp);
               currentDir = tmp;
               //found = true;
               break;
            }else if((currentDir[i].attributes & 0x20) == 0x20){
               
               //file case
               fileHeader_t tmpFile = currentDir[i];
               currentDir = rootFiles;
               found = true;
               return tmpFile;

            }

         }

         i += 1;

      }

   }

   //one file takes 4x8bits, first file should be ignored cause that's the root dir, so i < 8 is 3 files max
   for (uint8_t i = 1; i < 24; i++) {

      //THIS IS THE CORRECT WAY OF ACCESSING
      //ONLY USE &rootFiles[i] IF YOU WANT THE ADDRESS OF FILE AT THAT INDEX
      if(rootFiles[i].attributes == 0xE5) {kprintf("fat: deleted entry\n"); continue;}
      if(rootFiles[i].attributes == 0x0F) {continue;}
      if(rootFiles[i].attributes == 0x00) {kprintf("fat: end of dir at indx: %d\n", i); break;}

      uint8_t name[13] = {0};
      fileHeader_t* f;
      uint8_t lengthF = getFileName(&rootFiles[i], name);

      //kprintf("name: %s\n", name);

      if (strcmp(name, fileName) == 0) {

         kprintf("fat: found file %s\n", name);
         return rootFiles[i];

      }
   }

   fileHeader_t didNotFind;
   didNotFind.fileSize = -1;
   return didNotFind;

}

//function takes in index of first cluster, then loads that cluster to loadAddr, increment with clustersize and load next cluster in chain
uint8_t* loadClusterChain(uint16_t firstCluster, fileHeader_t* loadAddr){

   uint64_t addr = (uint64_t)loadAddr;
   uint32_t currentCluster = (uint32_t)firstCluster;

   uint8_t done = 0;
   do {
   
      //1. load cluster
      ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(currentCluster), 0x10, (uint32_t)addr);

      //2. increment loadAddr
      //addr += 0x200;
      addr = addr + ( bootsect.sectsPerCluster * bootsect.bytesPerSect);

      //3. find whatever the next currentcluster should be
      uint32_t nextFat = fat[currentCluster] & 0x0FFFFFF; //this should mask it so we ignore the top 4 reserved bits
      if(nextFat == 0x0FFFFFF){ done = 1; kprintf("fat: hit end of file\n"); break;} //special case: end of cluster chain
      currentCluster = nextFat;


   }while (!done);
   kprintf("fat: loaded file\n");
   return (uint8_t*)loadAddr;

}

//used for loading actuall data, not fat info
uint32_t clusterToLba(uint32_t clusterIndx){

   return clusterBeginLba + (clusterIndx - 2) * bootsect.sectsPerCluster;

}

void recodeFileName(char* name, char* loadAddr){

   uint8_t i = 0;
   uint8_t j;

   while (i < 11) {

      if (name[i] == '.') {

         for(j = i; j < 8; j++){

            loadAddr[j] = 0x20;

         }

         i++;
         
         for (int k = 0; k < 3; k++) {

            loadAddr[j] = toupper(name[i]);
            i++;
            j++;
         
         }

         return;

      }else if (name[i] == 0) {

         return;
      
      }else {
      
         loadAddr[i] = toupper(name[i]);

      }

      i++;
   
   }

}

uint8_t getFileName(fileHeader_t* file, uint8_t* name){

   uint8_t length = 8;
   uint8_t* f = (uint8_t*) file;

   for (int8_t i = 7; i >= 0; i--) {

      if (file->name[i] == ' ') {

         length--;
      
      }else{
         break;
      }
   
   }

   for (uint8_t j = 0; j < length; j++) {
      //tolower is locale dependend and unsafe
     // name[j] = tolower(file->name[j]);
     
     //alternative: just do it yourself
     name[j] = file->name[j] | 0x20;
   
   }

   kprintf("attr: %h, ", file->attributes);

   if((file->attributes & 0x20) == 0x20){

      kprintf("f: ");

      name[length] = '.';
   
      for(uint8_t k = 0; k < 3; k++){

         name[length + k + 1] = tolower(file->extension[k]);

      }
      name[length + 4] = '\0';

      return length + 4;
   }else if((file->attributes & 0x10) == 0x10){
      kprintf("d: ");
      name[length] = '\0';
      return length + 1;
   }


}
