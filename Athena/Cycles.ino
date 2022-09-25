// Code to read and store mating cycles
// August-September 2022
// Lucas Wirz-Vitiuk

// Load memory library
#include <Preferences.h>

// Initiate memory
Preferences memory;
unsigned int cycles;

// Define memory functions

unsigned int readMemory() {
  memory.begin("Storage", false);
  cycles = memory.getUInt("cycles", 0);
  memory.end();
  return cycles;
}

void writeMemory(const char* address, unsigned int content) {
  memory.begin("Storage", false);
  memory.putUInt(address, content);
  memory.end();
}
