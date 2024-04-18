#include <stdint.h>
#include "io.h"

extern int _start() {

   ioInit();

   print("------------------------------------- C code ------------------------------------\0");
   print("connOS rules\0");

   return 0;
}

