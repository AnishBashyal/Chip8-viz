#include <iostream>
#include <fstream>
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
        0x12, 0x00  // JP 0x200 (loop)
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

    uint32_t start = SDL_GetTicks();
    uint32_t lastStep = start;
    SDL_Event event;
    while (SDL_GetTicks() - start < 8000) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                start = 0;
                break;
            }
        }
        if (start == 0) break;

        if (SDL_GetTicks() - lastStep >= 1000) {
            emulator.step();
            lastStep = SDL_GetTicks();
        }

        renderDisplay(renderer, emulator);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
