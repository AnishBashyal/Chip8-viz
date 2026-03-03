#include "Chip8.h"
#include <fstream>
#include <vector>
#include <iostream>

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

void Chip8::decodeInstruction() {
    uint8_t firstNibble = (opcode & 0xF000) >> 12;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = opcode & 0x000F;
    uint8_t kk = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;

    std::cout << "Decoding opcode 0x" << std::hex << opcode << std::dec << "\n";
    std::cout << "  first nibble: 0x" << std::hex << static_cast<unsigned>(firstNibble) << std::dec << "\n";
    std::cout << "  x: V" << static_cast<unsigned>(x)
              << "  y: V" << static_cast<unsigned>(y)
              << "  n: " << static_cast<unsigned>(n)
              << "  kk: 0x" << std::hex << static_cast<unsigned>(kk)
              << "  nnn: 0x" << std::hex << nnn << std::dec << "\n";
}

void Chip8::executeInstruction() {
    uint8_t firstNibble = (opcode & 0xF000) >> 12;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t kk = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;

    switch (firstNibble) {
        case 0xA:
            I = nnn;
            std::cout << "Executing: LD I, 0x" << std::hex << nnn << std::dec << "\n";
            std::cout << "  -> I now = " << I << "\n";
            break;

        case 0x6:
            V[x] = kk;
            std::cout << "Executing: LD V" << static_cast<unsigned>(x)
                      << ", 0x" << std::hex << static_cast<unsigned>(kk) << std::dec << "\n";
            std::cout << "  -> V" << static_cast<unsigned>(x)
                      << " now = " << static_cast<unsigned>(V[x]) << "\n";
            break;

        default:
            std::cout << "Execute: unimplemented opcode 0x"
                      << std::hex << opcode << std::dec << "\n";
            break;
    }
}

void Chip8::step() {
    fetchInstruction();
    decodeInstruction();
    executeInstruction();
}