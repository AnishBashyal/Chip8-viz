#include "DashboardFont.h"
#include <cctype>

namespace {

// 5×7 glyph rows (bit4 = left column). Nibbles 0–F.
constexpr uint8_t kHexGlyph[16][7] = {
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}, // 1
    {0x0E, 0x11, 0x01, 0x06, 0x08, 0x10, 0x1F}, // 2
    {0x0E, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0E}, // 3
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}, // 4
    {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E}, // 5
    {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E}, // 6
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}, // 7
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}, // 8
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C}, // 9
    {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}, // A
    {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}, // B
    {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}, // C
    {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C}, // D
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}, // E
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}, // F
};

void drawGlyph(SDL_Renderer* renderer, int px, int py, int zoom, const uint8_t rows[7]) {
    for (int r = 0; r < 7; ++r) {
        uint8_t bits = rows[r];
        for (int c = 0; c < 5; ++c) {
            if ((bits >> (4 - c)) & 1) {
                SDL_Rect cell = {px + c * zoom, py + r * zoom, zoom, zoom};
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }
}

const uint8_t* glyphForChar(char ch) {
    static constexpr uint8_t kBlank[7] = {0, 0, 0, 0, 0, 0, 0};
    static constexpr uint8_t kColon[7] = {0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00};
    static constexpr uint8_t kDash[7] = {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00};
    static constexpr uint8_t kPlus[7] = {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00};

    switch (std::toupper(static_cast<unsigned char>(ch))) {
        case 'A': return kHexGlyph[0xA];
        case 'B': return kHexGlyph[0xB];
        case 'C': return kHexGlyph[0xC];
        case 'D': return kHexGlyph[0xD];
        case 'E': return kHexGlyph[0xE];
        case 'F': return kHexGlyph[0xF];
        case 'G': {
            static constexpr uint8_t g[7] = {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0E};
            return g;
        }
        case 'H': {
            static constexpr uint8_t h[7] = {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
            return h;
        }
        case 'I': {
            static constexpr uint8_t i[7] = {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F};
            return i;
        }
        case 'L': {
            static constexpr uint8_t l[7] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F};
            return l;
        }
        case 'M': {
            static constexpr uint8_t m[7] = {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11};
            return m;
        }
        case 'O': return kHexGlyph[0x0];
        case 'P': {
            static constexpr uint8_t p[7] = {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10};
            return p;
        }
        case 'Q': {
            static constexpr uint8_t q[7] = {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D};
            return q;
        }
        case 'R': {
            static constexpr uint8_t r[7] = {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11};
            return r;
        }
        case 'S': {
            static constexpr uint8_t s[7] = {0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E};
            return s;
        }
        case 'T': {
            static constexpr uint8_t t[7] = {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
            return t;
        }
        case 'U': {
            static constexpr uint8_t u[7] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
            return u;
        }
        case 'V': {
            static constexpr uint8_t v[7] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04};
            return v;
        }
        case 'X': {
            static constexpr uint8_t x[7] = {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11};
            return x;
        }
        case 'Y': {
            static constexpr uint8_t y[7] = {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04};
            return y;
        }
        case '0': return kHexGlyph[0x0];
        case '1': return kHexGlyph[0x1];
        case '2': return kHexGlyph[0x2];
        case '3': return kHexGlyph[0x3];
        case '4': return kHexGlyph[0x4];
        case '5': return kHexGlyph[0x5];
        case '6': return kHexGlyph[0x6];
        case '7': return kHexGlyph[0x7];
        case '8': return kHexGlyph[0x8];
        case '9': return kHexGlyph[0x9];
        case ':': return kColon;
        case '-': return kDash;
        case '+': return kPlus;
        case ' ': return kBlank;
        default: return kBlank;
    }
}

} // namespace

void drawHexDigit(SDL_Renderer* renderer, int px, int py, int zoom, unsigned nibble) {
    nibble &= 0xF;
    drawGlyph(renderer, px, py, zoom, kHexGlyph[nibble]);
}

void drawHexByte(SDL_Renderer* renderer, int px, int py, int zoom, uint8_t v) {
    drawHexDigit(renderer, px, py, zoom, v >> 4);
    drawHexDigit(renderer, px + 6 * zoom, py, zoom, v & 0xF);
}

void drawHexWord(SDL_Renderer* renderer, int px, int py, int zoom, uint16_t v) {
    drawHexDigit(renderer, px, py, zoom, (v >> 12) & 0xF);
    drawHexDigit(renderer, px + 6 * zoom, py, zoom, (v >> 8) & 0xF);
    drawHexDigit(renderer, px + 12 * zoom, py, zoom, (v >> 4) & 0xF);
    drawHexDigit(renderer, px + 18 * zoom, py, zoom, v & 0xF);
}

void drawText(SDL_Renderer* renderer, int px, int py, int zoom, const char* text) {
    int cursor = px;
    for (const char* p = text; *p != '\0'; ++p) {
        const uint8_t* glyph = glyphForChar(*p);
        drawGlyph(renderer, cursor, py, zoom, glyph);
        cursor += 6 * zoom;
    }
}
