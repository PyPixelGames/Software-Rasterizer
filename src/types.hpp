#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

inline uint32_t BLACK = 0xFF000000u;
inline uint32_t WHITE = 0xFFFFFFFFu;
inline uint32_t GREEN = 0xFF00FF00u;
inline uint32_t BLUE = 0xFF0000FFu;
inline uint32_t RED = 0xFFFF0000u;

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
	short int x=0;
	short int y=0;
};

struct FPos2{
	float x=0.0f;
	float y=0.0f;
};

struct Pos3{
	short int x=0;
	short int y=0;
	short int z=0;
};

struct FPos3{
	float x=0.0f;
	float y=0.0f;
	float z=0.0f;
};

struct Screen {
    const int width = 800;
    const int height = 600;
	std::vector<uint32_t> pixelBuffer = std::vector<uint32_t>(width * height, 0xFF000000U);

	void clear(uint32_t color = 0xFF000000U){
        std::fill(pixelBuffer.begin(), pixelBuffer.end(), color);
    }
};

struct Viewport{
	const float width=2.0f;
	const float height=1.5f;
	const float distance=2.0f;
};

struct Vertex { float x, y, z; };
struct TextureCoord { float u, v; };
struct Normal { float x, y, z; };

struct ObjModel{
    std::string objName;
    std::vector<Vertex> vertices;
    std::vector<TextureCoord> texCoords;
    std::vector<Normal> normals;
};
