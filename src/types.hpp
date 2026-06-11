#pragma once
#include <vector>
#include <cstdint>
#include <cstring>

struct Screen {
    const int width = 800;
    const int height = 600;

    std::vector<uint32_t> pixelBuffer = std::vector<uint32_t>(width * height, 0xFF000000U);
};
