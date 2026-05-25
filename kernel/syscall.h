#include <stdint.h>

void setupSyscall(uint64_t VBEInfoBlockAddr);
uint64_t getVBEIBA();
void handleSyscall();
void handleKeyboard();
