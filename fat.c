#include "fat.h"
#include "io.h"
#include "ide.h"
#include <stdint.h>
#include "utils.h"

fatBootsector_t bootsect;
fatEbr32_t ebrsect;
uint32_t clusterBeginLba;
fileHeader_t rootdir;
fileHeader_t* rootFiles;

void fatInit(){

   fatBootsector_t* bootaddr = (fatBootsector_t*) BOOTADDR; //0x70000
   fatEbr32_t* ebraddr = (fatEbr32_t*)(BOOTADDR + sizeof(fatBootsector_t));
   fileHeader_t* rootaddr = (fileHeader_t*) ROOTADDR; //0x74000

   ide_read_sectors(0, 1, 0x000000800, 0x10, (uint32_t)bootaddr); //read VBR

   bootsect = *bootaddr;
   ebrsect = *ebraddr;
   rootdir = *rootaddr;
   
   clusterBeginLba = 0x000000800 + bootsect.numReservedSects + (bootsect.numFats * ebrsect.fatSize);

   //load root dir
   uint8_t status = ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(ebrsect.rootCluster), 0x10, ROOTADDR);
   rootFiles = rootaddr;

   
}

uint8_t openFile(char* fileName, fileHeader_t* loadAddr){

   for (uint8_t i = 2; i < 6; i += 2) {

      uint8_t name[12] = {0};
      uint8_t lengthF = getFileName(&rootFiles[i], name);

      if (strcmp(name, fileName) == 0) {

         return ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(rootFiles[i].startingCluster), 0x10, (uint32_t)loadAddr);
      
      }
   
   }

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
