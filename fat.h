#include <stdint.h>

typedef struct fatBootsector{

   uint8_t bootjump[3];
   uint8_t oem_name[8];
   uint16_t bytesPerSect;
   uint8_t sectsPerCluster;
   uint16_t numReservedSects;//usually 32 for fat 32, includes first sector
   uint8_t numFats;//almost always 2
   uint16_t numRootDirs;//0 on fat32
   uint16_t totalSectsSmall; //0 on fat 32
   uint8_t mediaDesc;
   uint16_t sectsPerFat;
   uint16_t sectsPerTrack;
   uint16_t headsPerSide;
   uint32_t numHiddenSects;
   uint32_t totalSectsLarge;

}__attribute__((packed)) fatBootsector_t;

typedef struct ebr32{

   uint32_t fatSize;
   uint16_t flags;
   uint16_t fatVersion;
   uint32_t rootCluster;//usually 2
   uint16_t fsinfoSect; //sector num of fsinfo
   uint16_t backupBootSect; //sector number for backup boot sector
   uint8_t reserved_0[12];
   uint8_t driveNum;
   uint8_t winNTflags;
   uint8_t signature; //28h || 29H
   uint32_t volumeID;
   uint8_t volumeLabel[11];
   uint8_t fatID[8]; //"FAT32   "

}__attribute__((packed)) fatEbr32_t;

typedef struct fileEntry{

   uint8_t name[8];
   uint8_t extension[3];
   uint8_t attributes;
   uint8_t reserved[10];
   uint16_t timeCreated;
   uint16_t dateCreated;
   uint16_t startingCluster;
   uint32_t fileSize;

}__attribute__((packed)) fileHeader_t;

#define BOOTADDR 0x70000
#define ROOTADDR 0x74000
void fatInit();
uint32_t clusterToLba(uint32_t clusterIndx);
