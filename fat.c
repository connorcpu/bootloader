#include "fat.h"
#include "io.h"
#include "ide.h"
#include <stdint.h>

fatBootsector_t bootsect;
fatEbr32_t ebrsect;
uint32_t clusterBeginLba;
fileHeader_t rootdir;

void fatInit(){

   fatBootsector_t* bootaddr = (fatBootsector_t*) BOOTADDR; //0x70000
   fatEbr32_t* ebraddr = (fatEbr32_t*)(BOOTADDR + sizeof(fatBootsector_t));
   fileHeader_t* rootaddr = (fileHeader_t*) ROOTADDR; //0x74000
   ide_read_sectors(0, 1, 0x000000800, 0x10, (uint32_t)bootaddr);

   bootsect = *bootaddr;
   ebrsect = *ebraddr;
   rootdir = *rootaddr;
   
   clusterBeginLba = 0x000000800 + bootsect.numReservedSects + (bootsect.numFats * ebrsect.fatSize);

   kprintf("oem name: %s\n", bootsect.oem_name);
   kprintf("file system type: %s\n", ebrsect.fatID);
   ebrsect.volumeLabel[10] = 0;
   kprintf("volume label: %s\n", ebrsect.volumeLabel);
   kprintf("resevered sectors: %d\n", bootsect.numReservedSects);
   //kprintf("fatSize: %d\n", ebrsect.fatSize);
   //kprintf("fats: %d\n", bootsect.numFats);
   //kprintf("drive num: %d\n", ebrsect.driveNum);
   //kprintf("available sectors: %d\n", bootsect.totalSectsLarge);
   //kprintf("numRootDirs: %d\n", bootsect.numRootDirs);
   //kprintf("rootDirCluster: %d\n", ebrsect.rootCluster);
   //kprintf("fsinfo location: %d\n", ebrsect.fsinfoSect);
   //kprintf("backupBootSect location: %d\n", ebrsect.backupBootSect);
   kprintf("reading rootdir...\n");
   //kprintf("clusterBeginLba: %d\n", clusterBeginLba);
   //load root dir
   uint8_t status = ide_read_sectors(0, bootsect.sectsPerCluster, clusterToLba(2), 0x10, ROOTADDR);
   kprintf("read status: %d\n", status);

   fileHeader_t* rootFiles = rootaddr;

   for (int i = 0; i < 6; i += 2) {

      kprintf("file: %s\n", rootFiles[i].name);
   
   }
   
}

uint32_t clusterToLba(uint32_t clusterIndx){

   return clusterBeginLba + (clusterIndx - 2) * bootsect.sectsPerCluster;

}
