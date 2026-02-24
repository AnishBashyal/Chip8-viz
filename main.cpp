#include <iostream>
#include <iomanip>
#include <fstream>
#include "Chip8.h"

void createDummyROM() {
    std::ofstream file("dummy_rom.ch8", std::ios::binary);
    uint8_t bytes[] = {0xA2, 0x2A, 0x60, 0x0C};
    file.write(reinterpret_cast<char*>(bytes), sizeof(bytes));
    file.close();
}

int main() {
    std::cout << "Initializing system...\n";
    Chip8 emulator;

    createDummyROM();

    if (emulator.loadROM("dummy_rom.ch8")) {
        std::cout << "ROM loaded successfully.\n\n";
        
        emulator.fetchInstruction();
        std::cout << "Instruction 1: 0x" << std::hex << emulator.opcode << "\n";

        emulator.fetchInstruction();
        std::cout << "Instruction 2: 0x" << std::hex << emulator.opcode << "\n";
    } else {
        std::cout << "Failed to load ROM.\n";
    }

    return 0;
}