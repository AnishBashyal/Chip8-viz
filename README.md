# Chip-8 Viz

## Project Description

Chip-8 Viz is an interactive educational emulator designed to help students understand computer architecture by visualizing the internal state of a CPU. Currently, the project is in its initial "headless" stage. The codebase successfully defines the core CPU hardware (4KB memory map, registers, stack) and implements the logic to load binary game ROMs and fetch 16-bit machine instructions using proper bitwise operations.

## Technologies Used

- C++
- CMake (for build configuration)

## Setup Instructions

To set up the project on a macOS or Linux system, you need to have a C++ compiler and CMake installed.

1. Clone this repository to your local machine.
2. Open your terminal and navigate into the project folder: `cd chip8-viz`
3. Create a dedicated directory for the compiled files: `mkdir build`
4. Move into the build directory: `cd build`
5. Generate the build configuration using CMake: `cmake ..`
6. Compile the source code: `make`

## How to Run the Project

After compiling, stay inside the `build` directory and run one of these:

- `./chip8` (runs the built-in dummy ROM demo)
- `./chip8 path/to/rom.ch8` (runs a specific Chip-8 ROM file)

If ROM loading fails, the program prints the ROM path and usage examples so you can quickly correct the command.
