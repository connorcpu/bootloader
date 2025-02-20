#include "fat.h"
#include "io.h"
#include "ide.h"
#include <stdint.h>
#include "utils.h"
#include "memory.h"

fatBootsector_t bootsect;
fatEbr32_t ebrsect;
uint32_t clusterBeginLba;
fileHeader_t rootdir;
fileHeader_t* rootFiles;
uint32_t pointer;

void fatInit(){

   kmalloc(512, (uint32_t*) &pointer);

   fatBootsector_t* bootaddr = (fatBootsector_t*) pointer;
   fatEbr32_t* ebraddr = (fatEbr32_t*) (pointer + sizeof(fatBootsector_t));

   ide_read_sectors(0, 1, 0x000000800, 0x10, (uint32_t)bootaddr); //read VBR

   bootsect = *bootaddr;
   ebrsect = *ebraddr;

//   kprintf("bytes per sector: %d\n", bootsect.bytesPerSect);

   fileHeader_t* rootaddr = (fileHeader_t*)kmalloc(bootsect.sectsPerCluster * bootsect.bytesPerSect, 0);
   rootdir = *rootaddr;
   
   clusterBeginLba = 0x000000800 + bootsect.numReservedSects + (bootsect.numFats * ebrsect.fatSize);

   //load root dir
   uint8_t status = ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(ebrsect.rootCluster), 0x10, (uint32_t)rootaddr);
   rootFiles = rootaddr;
   
}

uint8_t openFile(char* fileName, fileHeader_t* loadAddr){

/*   bool found = false;
   fileHeader_t* currentDir = rootFiles;
   uint8_t toSearch[] = fileName;
   uint8_t searching[12] = {0};

   while(!found){

      //for(uint8_t j = 1; j < len(searchName); j++){
      for(uint8_t j = 1; j < 11; j++){

         if (searchName[i] == '/') {
         
            break;
         }

         searching[i] = searchName[i + 1];

      }

      uint16_t i = 0;
      while(currentDir[i] != 0){

         uint8_t name[12] = {0};
         getFileName(&currentDir[i], name);

         if (strcmp(name, fileName) == 0) {
         
            //found the file
//            return loadClusterChain();
            return ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(rootFiles[i].startingCluster), 0x10, (uint32_t)loadAddr);

         }

         i++;

      }

   }
*/
   //one file takes 2x8bits, first file should be ignored cause that's the root dir, so i < 8 is 3 files max
   for (uint8_t i = 2; i < 8; i += 2) {

      uint8_t name[12] = {0};
      uint8_t lengthF = getFileName(&rootFiles[i], name);

      if (strcmp(name, fileName) == 0) {

         return ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(rootFiles[i].startingCluster), 0x10, (uint32_t)loadAddr);
      
      }
   
   }

}

//function takes in index of first cluster, then loads that cluster to loadAddr, increment with clustersize and load next cluster in chain
uint8_t loadClusterChain(uint32_t firstCluster, fileHeader_t* loadAddr){
   
   fileHeader_t* addr = loadAddr;
   uint32_t currentCluster = firstCluster;
   //1. load cluster
   //2. increment loadAddr 
   //3. load next cluster

   do {
   
      ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(currentCluster), 0x10, (uint32_t)addr);

   }while (0);

}

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

   for (uint8_t i = 7; i > 0; i--) {

      if (file->name[i] == ' ') {

         length--;
      
      }
   
   }

   for (uint8_t j = 0; j < length; j++) {

      name[j] = tolower(file->name[j]);
   
   }

   name[length] = '.';
   
   for(uint8_t k = 0; k < 3; k++){

      name[length + k + 1] = tolower(file->extension[k]);

   }

   name[length + 5] = 0;

   return length + 4;

}
