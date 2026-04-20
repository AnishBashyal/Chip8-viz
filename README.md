# Chip-8 Viz

## Project Description

Chip-8 Viz is an interactive educational emulator: a Chip-8 virtual machine with SDL2 for a window, scaled 64×32 display, keypad input, timers, and basic sound. The core is a fetch–decode–execute loop with a 4 KB memory map, registers, and stack.

## Technologies Used

- C++17
- CMake
- SDL2 (video, events, audio)

## Setup Instructions

To set up the project on a macOS or Linux system, you need to have a C++ compiler and CMake installed.

1. Clone this repository to your local machine.
2. Open your terminal and navigate into the project folder: `cd chip8-viz`
3. Create a dedicated directory for the compiled files: `mkdir build`
4. Move into the build directory: `cd build`
5. Generate the build configuration using CMake: `cmake ..`
6. Compile the source code: `cmake --build .` (or `make`)

You need SDL2 installed and discoverable by CMake (`find_package(SDL2)`).

## How to Run the Project

After compiling, stay inside the `build` directory and run one of these:

- `./chip8` — runs the built-in dummy ROM demo
- `./chip8 path/to/rom.ch8` — runs a Chip-8 ROM from disk

If ROM loading fails, the program prints the path and usage hints.

## Controls

| Key | Action |
| --- | --- |
| `1`–`4`, `Q` `W` `E` `R`, `A` `S` `D` `F`, `Z` `X` `C` `V` | Chip-8 keypad (16 keys) |
| `Esc` | Quit |
| `P` | Pause / resume emulation (timers and CPU stop while paused) |
| `N` | While paused: execute **one** `step()` (single instruction) |

The window title shows live `DT` / `ST`. It may also show `[PAUSED]` or `[WAIT KEY]` (when a ROM executes `LD Vx, K` / `FX0A` and is waiting for a key).
