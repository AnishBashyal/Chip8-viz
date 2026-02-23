#include "Chip8.h"

Chip8::Chip8() {
    // The program counter starts at 0x200
    pc = 0x200;
    
    // Clear memory, registers, and stack
    memory.fill(0);
    V.fill(0);
    stack.fill(0);
    
    I = 0;
    sp = 0;
}