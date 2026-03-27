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

uint8_t* fat;

void fatInit(){

//   kmalloc(512, (uint32_t*) &pointer);
//
   kprintf("initing fat\n");

   pointer = kmalloc(512);

   fatBootsector_t* bootaddr = (fatBootsector_t*) pointer;
   fatEbr32_t* ebraddr = (fatEbr32_t*) (pointer + sizeof(fatBootsector_t));

   ide_read_sectors(0, 1, 0x000000800, 0x10, (uint32_t)bootaddr); //read VBR

   bootsect = *bootaddr;
   ebrsect = *ebraddr;

//   kprintf("bytes per sector: %d\n", bootsect.bytesPerSect);
   //kprintf("LBA start: %h\n", bootsect.);

   fileHeader_t* rootaddr = (fileHeader_t*)kmalloc(bootsect.sectsPerCluster * bootsect.bytesPerSect);
   rootdir = *rootaddr;
   
   clusterBeginLba = 0x000000800 + bootsect.numReservedSects + (bootsect.numFats * ebrsect.fatSize);

   fat = kmalloc(ebrsect.fatSize * bootsect.bytesPerSect);
   //read the fat 
   //mapPage((uint8_t*)0x1001200, (uint8_t*)0x1001200, 0x0);
   //mapPage((uint8_t*)0x1002200, (uint8_t*)0x1002200, 0x0);

/*   for(uint8_t i = 0; i < 2; i++){
      uint8_t* mapAddr = (uint8_t*)0x1001200 + (i * 0x1000);
      mapPage((uint8_t*)mapAddr, (uint8_t*)mapAddr, 0x0);

   }*/
   mapPage((uint8_t*)0x1ff0000, (uint8_t*)0x1ff0000, 0x0);
   /*for(uint8_t i = 0; i < 2; i++){
      uint8_t* mapAddr = (uint8_t*)0x1ff1000 + (i * 0x1000);
      mapPage((uint8_t*)mapAddr, (uint8_t*)mapAddr, 0x0);

   }*/
   //ide_read_sectors(0, ebrsect.fatSize * bootsect.bytesPerSect, 0x800 + bootsect.numReservedSects, 0x10, (uint32_t)fat); 
   ide_read_sectors(0, ebrsect.fatSize, 0x800 + bootsect.numReservedSects, 0x10, (uint32_t)fat); 


   //load root dir
   uint8_t status = ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(ebrsect.rootCluster), 0x10, (uint32_t)rootaddr);
   rootFiles = rootaddr;
   kprintf("loading root dir at %h\n", (uint64_t)rootFiles);
   
   kprintf("bytes per sect: %d; sects per cluster: %d\n", bootsect.bytesPerSect, bootsect.sectsPerCluster);
   
}

int8_t openFile(char* fileName, fileHeader_t* loadAddr){

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
   //one file takes 4x8bits, first file should be ignored cause that's the root dir, so i < 8 is 3 files max
   for (uint8_t i = 0; i < 16; i++) {

      if(rootFiles[i].attributes == 0xE5) {kprintf("deleted entry\n"); continue;}
      if(rootFiles[i].attributes == 0x0F) {kprintf("long name entry\n"); continue;}
      if(rootFiles[i].attributes == 0x00) {kprintf("end of dir\n"); /*break;*/}

      kprintf("reading from %h\n", &rootFiles[i]);
      uint8_t name[13] = {0};
      fileHeader_t* f;
      //kprintf("f addr: %h\n", &f);
     // memcpy(f, rootFiles + (i * sizeof(fileHeader_t)), sizeof(fileHeader_t));
      uint8_t lengthF = getFileName(&rootFiles[i], name);
      //uint8_t lengthF = getFileName(f, name);

      kprintf("name: %s\n", name);

      if (strcmp(name, fileName) == 0) {

         kprintf("found file");
         return loadClusterChain(&rootFiles[i].startingCluster, (void *)loadAddr);
        // return ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(rootFiles[i].startingCluster), 0x10, (uint32_t)loadAddr);
      
      }
   }
   kprintf("file not found\n", fileName);
   return -1;

}

//function takes in index of first cluster, then loads that cluster to loadAddr, increment with clustersize and load next cluster in chain
uint8_t loadClusterChain(uint16_t* firstCluster, fileHeader_t* loadAddr){
   
   fileHeader_t* addr = loadAddr;
   uint32_t currentCluster = (uint32_t)firstCluster;
   kprintf("fat test %i\n", fat[currentCluster]);
   //1. load cluster
   //2. increment loadAddr 
   //3. load next cluster

   do {
   
      ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(currentCluster), 0x10, (uint32_t)addr);

   }while (0);

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

   kprintf("file addr: %h\n", file);
   kprintf("addr: %h\n", f);
   
   for(uint8_t j = 0; j < 8; j++){

      //kprintf("j: %d\n", j);
      //kprintf("value: %c, at %h\n", *(f+j), &(f[j]));
      kprintf("value: %c, at %h\n", *f, f);
      f++;
      
   }

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

   name[length] = '.';
   
   for(uint8_t k = 0; k < 3; k++){

      name[length + k + 1] = tolower(file->extension[k]);

   }

   name[length + 4] = '\0';

   return length + 4;

}
