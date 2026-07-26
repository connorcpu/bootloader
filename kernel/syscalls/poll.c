#include "poll.h"
#include "../io.h"
#include "../syscall.h"

uint16_t sysPoll(pollfd_t *fds, uint8_t nfds, uint16_t timeout){


   for(uint8_t i = 0; i < nfds; i++){

      if(fds[i].fd == 0){

         //while(getKeyboard() == false){
         //}

         return 1;

      }

   }

   return -1;

}
