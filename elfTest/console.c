#include "../stdlib/syscalls.h"
#include "../stdlib/stdio.h"

uint8_t fd;
uint8_t keyfd;

pollfd_t pfd;
char buff[1];

void _start(){

   kprintf("executing console\n");   

   fd = open("/dev/tty1", 0x0, 0x0);
   kprintf("got %i\n", fd);
   if(fd == 255){
      kprintf("failed to open tty1\n"); 
      exit(1);
      return;
   }

   keyfd = open("/dev/keyboard", 0x0, 0x0);

   pfd.fd = 0;
   pfd.events = 0x001;

   buff[0] = 0x00;

   write(fd, "AutismOS > ", 11);

   uint8_t linenum = 0;

   while(true){

      //if(poll(&pfd, 1, 0)){

      uint64_t red = read(keyfd, buff, 1);
         if(red != 0){
            if(buff[0] <= 0x7f && buff[0] != 0x00 && buff[0] >= 0x00){
               if(buff[0] == 0x1c){
                  //enter
                  ioctl(fd, 0, (linenum + 1) * (1920/8));
                  write(fd, "AutismOS > ", 11);
                  linenum++;

               }else if(buff[0] == 0x0e){
                  //backspace
                  ioctl(fd, 2, 1);
                  write(fd, " ", 1);
                  ioctl(fd, 2, 1);
               }else{
                  buff[0] = k2a(buff[0]);

                  write(fd, buff, 1);
               }
            }
            buff[0] = 0x00;

         }

      //}

   }

}
