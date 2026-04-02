#include "Chip8.h"
#include <fstream>
#include <vector>
#include <iostream>

Chip8::Chip8() {
    // The program counter starts at 0x200
    pc = 0x200;
    
    // Clear memory, registers, and stack
    memory.fill(0);
    display.fill(0);
    V.fill(0);
    stack.fill(0);
    
    I = 0;
    sp = 0;
    opcode = 0;

    delayTimer = 0;
    soundTimer = 0;
}

void Chip8::advanceTimers() {
    if (delayTimer > 0) {
        delayTimer--;
    }
    if (soundTimer > 0) {
        soundTimer--;
    }
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
    if (!trace) return;

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
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t n = opcode & 0x000F;

    switch (firstNibble) {
        case 0x0:
            if (opcode == 0x00E0) {
                display.fill(0);
                if (trace) std::cout << "Executing: CLS\n";
            } else if (opcode == 0x00EE) {
                sp--;
                pc = stack[sp];
                if (trace) {
                    std::cout << "Executing: RET\n";
                    std::cout << "  -> pc now = " << pc << "\n";
                }
            } else {
                if (trace) {
                    std::cout << "Execute: unimplemented opcode 0x"
                              << std::hex << opcode << std::dec << "\n";
                }
            }
            break;

        case 0x1:
            pc = nnn;
            if (trace) {
                std::cout << "Executing: JP 0x" << std::hex << nnn << std::dec << "\n";
                std::cout << "  -> pc now = " << pc << "\n";
            }
            break;

        case 0x2:
            stack[sp] = pc;
            sp++;
            pc = nnn;
            if (trace) {
                std::cout << "Executing: CALL 0x" << std::hex << nnn << std::dec << "\n";
                std::cout << "  -> pc now = " << pc << "\n";
            }
            break;

        case 0xA:
            I = nnn;
            if (trace) {
                std::cout << "Executing: LD I, 0x" << std::hex << nnn << std::dec << "\n";
                std::cout << "  -> I now = " << I << "\n";
            }
            break;

        case 0x6:
            V[x] = kk;
            if (trace) {
                std::cout << "Executing: LD V" << static_cast<unsigned>(x)
                          << ", 0x" << std::hex << static_cast<unsigned>(kk) << std::dec << "\n";
                std::cout << "  -> V" << static_cast<unsigned>(x)
                          << " now = " << static_cast<unsigned>(V[x]) << "\n";
            }
            break;

        case 0x7:
            V[x] = static_cast<uint8_t>(V[x] + kk);
            if (trace) {
                std::cout << "Executing: ADD V" << static_cast<unsigned>(x)
                          << ", 0x" << std::hex << static_cast<unsigned>(kk) << std::dec << "\n";
                std::cout << "  -> V" << static_cast<unsigned>(x)
                          << " now = " << static_cast<unsigned>(V[x]) << "\n";
            }
            break;

        case 0xD: {
            // DXYN: DRW Vx, Vy, nibble
            uint8_t vx = V[x];
            uint8_t vy = V[y];
            V[0xF] = 0;

            for (int row = 0; row < static_cast<int>(n); ++row) {
                uint8_t spriteByte = memory[I + row];
                for (int bit = 0; bit < 8; ++bit) {
                    if ((spriteByte & (0x80 >> bit)) != 0) {
                        int px = (static_cast<int>(vx) + bit) % 64;
                        int py = (static_cast<int>(vy) + row) % 32;
                        int idx = py * 64 + px;

                        if (display[idx] == 1) {
                            V[0xF] = 1;
                        }
                        display[idx] ^= 1;
                    }
                }
            }

            if (trace) {
                std::cout << "Executing: DRW V" << static_cast<unsigned>(x)
                          << ", V" << static_cast<unsigned>(y)
                          << ", " << static_cast<unsigned>(n) << "\n";
                std::cout << "  -> VF=" << static_cast<unsigned>(V[0xF]) << "\n";
            }
            break;
        }

        default:
            if (trace) {
                std::cout << "Execute: unimplemented opcode 0x"
                          << std::hex << opcode << std::dec << "\n";
            }
            break;
    }
}

void Chip8::step() {
    fetchInstruction();
    decodeInstruction();
    executeInstruction();

    if (trace) {
        std::cout << "STATE pc=0x" << std::hex << pc
                  << " opcode=0x" << opcode
                  << " I=0x" << I
                  << " sp=" << std::dec << static_cast<unsigned>(sp)
                  << " V0=" << static_cast<unsigned>(V[0])
                  << "\n\n";
    }
}