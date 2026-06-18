#pragma once
#include <vector>
#include <cstdint>
#include <cstring>

inline uint32_t BLACK = 0xFF000000u;
inline uint32_t WHITE = 0xFFFFFFFFu;
inline uint32_t GREEN = 0xFF00FF00u;
inline uint32_t BLUE = 0xFFFF0000u;
inline uint32_t RED = 0xFF0000FFu;

inline uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255){
    return (a << 24) | (r << 16) | (g << 8) | b;
}

inline void GetColor(uint32_t color, int& r, int& g, int& b, int& a) {
    r = (color >> 16) & 0xFF;
    g = (color >> 8)  & 0xFF;
    b = (color)       & 0xFF;
    a = (color >> 24) & 0xFF;
}

struct Pos2 {
	int x=0;
	int y=0;
};

struct FPos2{
	float x=0.0f;
	float y=0.0f;
};

struct Screen {
    const int width = 800;
    const int height = 600;
	std::vector<uint32_t> pixelBuffer = std::vector<uint32_t>(width * height, 0xFF000000U);
};
