cmake_minimum_required(VERSION 3.10)
project(Chip8Viz)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(chip8 main.cpp Chip8.cpp)
target_link_libraries(chip8 PRIVATE SDL2::SDL2)