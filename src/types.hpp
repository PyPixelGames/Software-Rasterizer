#pragma once
#include <vector>
#include <cstdint>
#include <cstring>

inline uint32_t BLACK = 0xFF000000u;
inline uint32_t WHITE = 0xFFFFFFFFu;

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
