#pragma once
#include <cstdint>
#include <array>
#include <string> 

class Chip8 {
public:
    // Memory
    std::array<uint8_t, 4096> memory{};

    // Display (64x32 monochrome)
    std::array<uint8_t, 64 * 32> display{};

    // Registers
    std::array<uint8_t, 16> V{};     // 16 General purpose registers (V0 - VF)
    uint16_t I{0};                   // Index register
    uint16_t pc{0x200};              // Program Counter

    // Stack
    std::array<uint16_t, 16> stack{};
    uint8_t sp{0};                   // Stack Pointer
    
    uint16_t opcode{0};              // Current instruction

    bool trace{true};                // if true, print decode/execute/STATE each step

    uint8_t delayTimer{0};
    uint8_t soundTimer{0};

    std::array<uint8_t, 16> keys{};

    bool waitingKey{false};
    uint8_t waitingReg{0};

    Chip8();
    bool loadROM(const std::string& filename);
    void fetchInstruction();
    void decodeInstruction();

    void executeInstruction();
    void step();

    void advanceTimers();

    void setKey(uint8_t keyIndex, bool pressed);
};
