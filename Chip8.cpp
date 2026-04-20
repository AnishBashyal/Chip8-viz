#include "Chip8.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdlib>

namespace {
constexpr uint16_t kFontBase = 0x50;
constexpr uint8_t kFontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
}

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

    keys.fill(0);

    waitingKey = false;
    waitingReg = 0;

    for (size_t i = 0; i < sizeof(kFontset); ++i) {
        memory[kFontBase + i] = kFontset[i];
    }
}

void Chip8::setKey(uint8_t keyIndex, bool pressed) {
    if (keyIndex >= 16) {
        return;
    }
    keys[keyIndex] = pressed ? 1 : 0;
    if (waitingKey && pressed) {
        V[waitingReg] = keyIndex;
        waitingKey = false;
        pc += 2;
    }
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

        case 0x3:
            if (V[x] == kk) {
                pc += 2;
            }
            if (trace) {
                std::cout << "Executing: SE V" << static_cast<unsigned>(x)
                          << ", 0x" << std::hex << static_cast<unsigned>(kk) << std::dec << "\n";
                std::cout << "  -> pc now = " << pc << "\n";
            }
            break;

        case 0x4:
            if (V[x] != kk) {
                pc += 2;
            }
            if (trace) {
                std::cout << "Executing: SNE V" << static_cast<unsigned>(x)
                          << ", 0x" << std::hex << static_cast<unsigned>(kk) << std::dec << "\n";
                std::cout << "  -> pc now = " << pc << "\n";
            }
            break;

        case 0x5:
            if (n == 0x0) {
                if (V[x] == V[y]) {
                    pc += 2;
                }
                if (trace) {
                    std::cout << "Executing: SE V" << static_cast<unsigned>(x)
                              << ", V" << static_cast<unsigned>(y) << "\n";
                    std::cout << "  -> pc now = " << pc << "\n";
                }
            } else if (trace) {
                std::cout << "Execute: unimplemented opcode 0x"
                          << std::hex << opcode << std::dec << "\n";
            }
            break;

        case 0xA:
            I = nnn;
            if (trace) {
                std::cout << "Executing: LD I, 0x" << std::hex << nnn << std::dec << "\n";
                std::cout << "  -> I now = " << I << "\n";
            }
            break;

        case 0x8:
            switch (n) {
                case 0x0:
                    V[x] = V[y];
                    if (trace) std::cout << "Executing: LD V" << static_cast<unsigned>(x)
                                         << ", V" << static_cast<unsigned>(y) << "\n";
                    break;
                case 0x1:
                    V[x] |= V[y];
                    if (trace) std::cout << "Executing: OR V" << static_cast<unsigned>(x)
                                         << ", V" << static_cast<unsigned>(y) << "\n";
                    break;
                case 0x2:
                    V[x] &= V[y];
                    if (trace) std::cout << "Executing: AND V" << static_cast<unsigned>(x)
                                         << ", V" << static_cast<unsigned>(y) << "\n";
                    break;
                case 0x3:
                    V[x] ^= V[y];
                    if (trace) std::cout << "Executing: XOR V" << static_cast<unsigned>(x)
                                         << ", V" << static_cast<unsigned>(y) << "\n";
                    break;
                case 0x4: {
                    uint16_t sum = static_cast<uint16_t>(V[x]) + static_cast<uint16_t>(V[y]);
                    V[0xF] = (sum > 0xFF) ? 1 : 0;
                    V[x] = static_cast<uint8_t>(sum & 0xFF);
                    if (trace) std::cout << "Executing: ADD V" << static_cast<unsigned>(x)
                                         << ", V" << static_cast<unsigned>(y) << " (VF carry)\n";
                    break;
                }
                case 0x5:
                    V[0xF] = (V[x] >= V[y]) ? 1 : 0;
                    V[x] = static_cast<uint8_t>(V[x] - V[y]);
                    if (trace) std::cout << "Executing: SUB V" << static_cast<unsigned>(x)
                                         << ", V" << static_cast<unsigned>(y) << " (VF no-borrow)\n";
                    break;
                case 0x6: {
                    uint8_t src = quirkShiftVY ? V[y] : V[x];
                    V[0xF] = src & 0x1;
                    V[x] = static_cast<uint8_t>(src >> 1);
                    if (trace) {
                        std::cout << "Executing: SHR V" << static_cast<unsigned>(x)
                                  << (quirkShiftVY ? " from Vy" : "")
                                  << " (VF=LSB)\n";
                    }
                    break;
                }
                case 0x7:
                    V[0xF] = (V[y] >= V[x]) ? 1 : 0;
                    V[x] = static_cast<uint8_t>(V[y] - V[x]);
                    if (trace) std::cout << "Executing: SUBN V" << static_cast<unsigned>(x)
                                         << ", V" << static_cast<unsigned>(y) << " (VF no-borrow)\n";
                    break;
                case 0xE: {
                    uint8_t src = quirkShiftVY ? V[y] : V[x];
                    V[0xF] = (src >> 7) & 0x1;
                    V[x] = static_cast<uint8_t>(src << 1);
                    if (trace) {
                        std::cout << "Executing: SHL V" << static_cast<unsigned>(x)
                                  << (quirkShiftVY ? " from Vy" : "")
                                  << " (VF=MSB)\n";
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
            if (trace) {
                std::cout << "  -> V" << static_cast<unsigned>(x)
                          << "=" << static_cast<unsigned>(V[x])
                          << " VF=" << static_cast<unsigned>(V[0xF]) << "\n";
            }
            break;

        case 0x9:
            if (n == 0x0) {
                if (V[x] != V[y]) {
                    pc += 2;
                }
                if (trace) {
                    std::cout << "Executing: SNE V" << static_cast<unsigned>(x)
                              << ", V" << static_cast<unsigned>(y) << "\n";
                    std::cout << "  -> pc now = " << pc << "\n";
                }
            } else if (trace) {
                std::cout << "Execute: unimplemented opcode 0x"
                          << std::hex << opcode << std::dec << "\n";
            }
            break;

        case 0xB:
            pc = static_cast<uint16_t>(nnn + V[0]);
            if (trace) {
                std::cout << "Executing: JP V0, 0x" << std::hex << nnn << std::dec << "\n";
                std::cout << "  -> pc now = " << pc << "\n";
            }
            break;

        case 0xC:
            V[x] = static_cast<uint8_t>((std::rand() & 0xFF) & kk);
            if (trace) {
                std::cout << "Executing: RND V" << static_cast<unsigned>(x)
                          << ", 0x" << std::hex << static_cast<unsigned>(kk) << std::dec << "\n";
                std::cout << "  -> V" << static_cast<unsigned>(x)
                          << " now = " << static_cast<unsigned>(V[x]) << "\n";
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

        case 0xE: {
            uint8_t key = V[x] & 0x0F;
            if (kk == 0x9E) {
                if (keys[key] != 0) {
                    pc += 2;
                }
                if (trace) {
                    std::cout << "Executing: SKP V" << static_cast<unsigned>(x) << "\n";
                    std::cout << "  -> key " << static_cast<unsigned>(key)
                              << " pressed=" << (keys[key] != 0 ? 1 : 0)
                              << " pc=0x" << std::hex << pc << std::dec << "\n";
                }
            } else if (kk == 0xA1) {
                if (keys[key] == 0) {
                    pc += 2;
                }
                if (trace) {
                    std::cout << "Executing: SKNP V" << static_cast<unsigned>(x) << "\n";
                    std::cout << "  -> key " << static_cast<unsigned>(key)
                              << " pressed=" << (keys[key] != 0 ? 1 : 0)
                              << " pc=0x" << std::hex << pc << std::dec << "\n";
                }
            } else {
                if (trace) {
                    std::cout << "Execute: unimplemented opcode 0x"
                              << std::hex << opcode << std::dec << "\n";
                }
            }
            break;
        }

        case 0xF:
            if (kk == 0x07) {
                V[x] = delayTimer;
                if (trace) {
                    std::cout << "Executing: LD V" << static_cast<unsigned>(x) << ", DT\n";
                    std::cout << "  -> V" << static_cast<unsigned>(x)
                              << " now = " << static_cast<unsigned>(V[x]) << "\n";
                }
            } else if (kk == 0x0A) {
                waitingKey = true;
                waitingReg = x;
                pc -= 2;
                if (trace) {
                    std::cout << "Executing: LD V" << static_cast<unsigned>(x) << ", K (waiting)\n";
                    std::cout << "  -> pc=0x" << std::hex << pc << std::dec << "\n";
                }
            } else if (kk == 0x15) {
                delayTimer = V[x];
                if (trace) {
                    std::cout << "Executing: LD DT, V" << static_cast<unsigned>(x) << "\n";
                    std::cout << "  -> DT now = " << static_cast<unsigned>(delayTimer) << "\n";
                }
            } else if (kk == 0x18) {
                soundTimer = V[x];
                if (trace) {
                    std::cout << "Executing: LD ST, V" << static_cast<unsigned>(x) << "\n";
                    std::cout << "  -> ST now = " << static_cast<unsigned>(soundTimer) << "\n";
                }
            } else if (kk == 0x1E) {
                I = static_cast<uint16_t>(I + V[x]);
                if (trace) {
                    std::cout << "Executing: ADD I, V" << static_cast<unsigned>(x) << "\n";
                    std::cout << "  -> I now = 0x" << std::hex << I << std::dec << "\n";
                }
            } else if (kk == 0x29) {
                uint8_t digit = V[x] & 0x0F;
                I = static_cast<uint16_t>(kFontBase + digit * 5);
                if (trace) {
                    std::cout << "Executing: LD F, V" << static_cast<unsigned>(x) << "\n";
                    std::cout << "  -> I now = 0x" << std::hex << I
                              << " (font for " << std::dec << static_cast<unsigned>(digit) << ")\n";
                }
            } else if (kk == 0x33) {
                uint8_t value = V[x];
                memory[I] = static_cast<uint8_t>(value / 100);
                memory[I + 1] = static_cast<uint8_t>((value / 10) % 10);
                memory[I + 2] = static_cast<uint8_t>(value % 10);
                if (trace) {
                    std::cout << "Executing: LD B, V" << static_cast<unsigned>(x) << "\n";
                    std::cout << "  -> [I..I+2] = "
                              << static_cast<unsigned>(memory[I]) << ", "
                              << static_cast<unsigned>(memory[I + 1]) << ", "
                              << static_cast<unsigned>(memory[I + 2]) << "\n";
                }
            } else if (kk == 0x55) {
                for (uint8_t reg = 0; reg <= x; ++reg) {
                    memory[I + reg] = V[reg];
                }
                if (quirkMemoryIncrementI) {
                    I = static_cast<uint16_t>(I + x + 1);
                }
                if (trace) {
                    std::cout << "Executing: LD [I], V0..V" << static_cast<unsigned>(x) << "\n";
                    if (quirkMemoryIncrementI) {
                        std::cout << "  -> I now = 0x" << std::hex << I << std::dec << "\n";
                    }
                }
            } else if (kk == 0x65) {
                for (uint8_t reg = 0; reg <= x; ++reg) {
                    V[reg] = memory[I + reg];
                }
                if (quirkMemoryIncrementI) {
                    I = static_cast<uint16_t>(I + x + 1);
                }
                if (trace) {
                    std::cout << "Executing: LD V0..V" << static_cast<unsigned>(x) << ", [I]\n";
                    if (quirkMemoryIncrementI) {
                        std::cout << "  -> I now = 0x" << std::hex << I << std::dec << "\n";
                    }
                }
            } else {
                if (trace) {
                    std::cout << "Execute: unimplemented opcode 0x"
                              << std::hex << opcode << std::dec << "\n";
                }
            }
            break;

        default:
            if (trace) {
                std::cout << "Execute: unimplemented opcode 0x"
                          << std::hex << opcode << std::dec << "\n";
            }
            break;
    }
}

void Chip8::step() {
    if (waitingKey) {
        return;
    }

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