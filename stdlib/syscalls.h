#include <stdint.h>

typedef struct pollfd{
   uint16_t fd;
   uint8_t events;
   uint8_t revents;

}__attribute__((packed)) pollfd_t;

uint8_t write(uint64_t fd, uint8_t* buf, uint64_t size);
uint8_t open(char* fileName, uint32_t flags, uint16_t mode);
uint8_t poll(pollfd_t* fds, uint8_t nfds, uint16_t timeout);
uint8_t exit(uint32_t errorCode);
