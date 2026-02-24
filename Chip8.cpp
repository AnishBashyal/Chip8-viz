#include "Chip8.h"
#include <fstream>
#include <vector>

Chip8::Chip8() {
    // The program counter starts at 0x200
    pc = 0x200;
    
    // Clear memory, registers, and stack
    memory.fill(0);
    V.fill(0);
    stack.fill(0);
    
    I = 0;
    sp = 0;
    opcode = 0;
}

bool Chip8::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        for (long i = 0; i < size; ++i) {
            memory[0x200 + i] = buffer[i];
        }
    }
    return true;
}

void Chip8::fetchInstruction() {
    // Shift the first byte left by 8, then bitwise OR with the second byte
    opcode = (memory[pc] << 8) | memory[pc + 1];
    
    // Increment the Program Counter by 2 to prepare for the next instruction
    pc += 2;
}