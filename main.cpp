#include <iostream>
#include <fstream>
#include <string>
#include <SDL.h>
#include "Chip8.h"

void createDummyROM() {
    std::ofstream file("dummy_rom.ch8", std::ios::binary);
    uint8_t bytes[] = {
        0x00, 0xE0, // CLS
        0x60, 0x0A, // LD V0, 0x0A
        0x61, 0x05, // LD V1, 0x05
        0xA3, 0x00, // LD I, 0x300
        0xD0, 0x15, // DRW V0, V1, 5
        0x62, 0xB4, // LD V2, 180 (~3s at 60 Hz)
        0xF2, 0x15, // LD DT, V2
        0x12, 0x0E  // JP 0x20E (idle while DT decrements)
    };
    file.write(reinterpret_cast<char*>(bytes), sizeof(bytes));
    file.close();
}

void renderDisplay(SDL_Renderer* renderer, const Chip8& emulator) {
    const int scale = 10;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (emulator.display[y * 64 + x] != 0) {
                SDL_Rect pixel = {x * scale, y * scale, scale, scale};
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

int mapSDLKeyToChip8(SDL_Keycode key) {
    switch (key) {
        case SDLK_1: return 0x1;
        case SDLK_2: return 0x2;
        case SDLK_3: return 0x3;
        case SDLK_4: return 0xC;
        case SDLK_q: return 0x4;
        case SDLK_w: return 0x5;
        case SDLK_e: return 0x6;
        case SDLK_r: return 0xD;
        case SDLK_a: return 0x7;
        case SDLK_s: return 0x8;
        case SDLK_d: return 0x9;
        case SDLK_f: return 0xE;
        case SDLK_z: return 0xA;
        case SDLK_x: return 0x0;
        case SDLK_c: return 0xB;
        case SDLK_v: return 0xF;
        default: return -1;
    }
}

int main() {
    std::cout << "Initializing system...\n";

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Chip8 Viz",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        320,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "SDL window opened.\n";

    Chip8 emulator;
    emulator.trace = false;
    createDummyROM();

    if (emulator.loadROM("dummy_rom.ch8")) {
        std::cout << "ROM loaded successfully.\n\n";
        for (int i = 0; i < 32; ++i) {
            emulator.display[i * 64 + i] = 1;
        }

        uint8_t sprite[5] = {0xF0, 0x90, 0x90, 0x90, 0xF0};
        for (int i = 0; i < 5; ++i) {
            emulator.memory[0x300 + i] = sprite[i];
        }
    } else {
        std::cout << "Failed to load ROM.\n";
    }

    bool running = true;
    SDL_Event event;
    Uint32 lastTicks = SDL_GetTicks();
    float timerAccumSec = 0.0f;
    float cpuAccumSec = 0.0f;
    constexpr float kTimerTickSec = 1.0f / 60.0f;
    constexpr float kCpuTickSec = 1.0f / 700.0f;

    while (running) {
        Uint32 now = SDL_GetTicks();
        float dtSec = (now - lastTicks) / 1000.0f;
        lastTicks = now;
        if (dtSec > 0.25f) {
            dtSec = 0.25f;
        }
        timerAccumSec += dtSec;
        cpuAccumSec += dtSec;
        while (timerAccumSec >= kTimerTickSec) {
            timerAccumSec -= kTimerTickSec;
            emulator.advanceTimers();
        }
        while (cpuAccumSec >= kCpuTickSec) {
            cpuAccumSec -= kCpuTickSec;
            emulator.step();
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    break;
                }
                int chip8Key = mapSDLKeyToChip8(event.key.keysym.sym);
                if (chip8Key >= 0) {
                    emulator.setKey(static_cast<uint8_t>(chip8Key), true);
                }
            } else if (event.type == SDL_KEYUP) {
                int chip8Key = mapSDLKeyToChip8(event.key.keysym.sym);
                if (chip8Key >= 0) {
                    emulator.setKey(static_cast<uint8_t>(chip8Key), false);
                }
            }
        }
        if (!running) break;

        std::string title = "Chip8 Viz | DT=" + std::to_string(emulator.delayTimer) +
                            " ST=" + std::to_string(emulator.soundTimer);
        SDL_SetWindowTitle(window, title.c_str());
        renderDisplay(renderer, emulator);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
