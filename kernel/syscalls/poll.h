#include <stdint.h>

typedef struct pollfd{
   uint16_t fd;
   uint8_t events;
   uint8_t revents;

}__attribute__((packed)) pollfd_t;

uint16_t sysPoll(pollfd_t *fds, uint8_t nfds, uint16_t timeout);
