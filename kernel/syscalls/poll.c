#include "poll.h"
#include "../io.h"
#include "../syscall.h"

uint16_t sysPoll(pollfd_t *fds, uint8_t nfds, uint16_t timeout){

   //temporary implementation because wtf is file devices 

//   kprintf("getting poll\n");
   if(getKeyboard()){

      return getScancode(); 

   }

   return -1;

}
