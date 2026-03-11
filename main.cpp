#include <iostream>
#include <iomanip>
#include <fstream>
#include "Chip8.h"

void createDummyROM() {
    std::ofstream file("dummy_rom.ch8", std::ios::binary);
    uint8_t bytes[] = {
        0x60, 0x01, // LD V0, 0x01
        0x22, 0x08, // CALL 0x208
        0x70, 0x01, // ADD V0, 0x01
        0x12, 0x00, // JP 0x200 (loop)

        0x70, 0x05, // ADD V0, 0x05  (subroutine at 0x208)
        0x00, 0xEE  // RET
    };
    file.write(reinterpret_cast<char*>(bytes), sizeof(bytes));
    file.close();
}

int main() {
    std::cout << "Initializing system...\n";
    Chip8 emulator;

    createDummyROM();

    if (emulator.loadROM("dummy_rom.ch8")) {
        std::cout << "ROM loaded successfully.\n\n";

        emulator.step();
        emulator.step();
        emulator.step();
        emulator.step();
        emulator.step();
        emulator.step();
    } else {
        std::cout << "Failed to load ROM.\n";
    }

    return 0;
}