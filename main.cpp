#include <iostream>
#include <iomanip>
#include <fstream>
#include <SDL.h>
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
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

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

    uint32_t start = SDL_GetTicks();
    SDL_Event event;
    while (SDL_GetTicks() - start < 5000) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                start = 0;
                break;
            }
        }
        if (start == 0) break;
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}