#pragma once
#include <SDL.h>
#include <cstdint>

// 5 columns × 7 rows per hex digit (rows top→bottom, bit4=left pixel).
void drawHexDigit(SDL_Renderer* renderer, int px, int py, int zoom, unsigned nibble);
void drawHexByte(SDL_Renderer* renderer, int px, int py, int zoom, uint8_t v);
void drawHexWord(SDL_Renderer* renderer, int px, int py, int zoom, uint16_t v);
