#include "../stdlib/syscalls.h"


void _start(){

   char* helloString = "Hello from C code!\n";

   write(1, helloString, sizeof(helloString));

}
