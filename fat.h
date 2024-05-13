#include <stdint.h>

typedef struct fatBootsector{

   uint8_t bootjump[3];
   uint8_t oem_name[8];
   uint16_t bytesPerSect;
   uint8_t sectsPerCluster;
   uint16_t numReservedSects;
   uint8_t numFats;
   uint16_t numRootDirs;
   uint16_t totalSectsSmall;
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
   uint32_t rootCluster;
   uint16_t fsinfoSect;
   uint16_t backupBootSect;
   uint8_t reserved_0[12];
   uint8_t driveNum;
   uint8_t winNTflags;
   uint8_t signature; //28h || 29H
   uint32_t volumeID;
   uint8_t volumeLabel[11];
   uint8_t fatID[8]; //"FAT32   "

}__attribute__((packed)) fatEbr32_t;
