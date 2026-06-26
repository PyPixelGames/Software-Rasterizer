#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <array>

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
	float y=0.0f; };

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

struct Vec4{
	float x=0;
	float y=0;
	float z=0;
	float w=0;
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

struct TextureCoord { float u, v; };

struct ObjModel{
    std::string objName;
    std::vector<FPos3> vertices;
    std::vector<TextureCoord> texCoords;
    std::vector<FPos3> normals;
    std::vector<std::vector<int>> tris;
};

using Mat4x4 = std::array<std::array<float, 4>, 4>;
constexpr Mat4x4 Identity4x4 = Mat4x4{{{1, 0, 0, 0}, {0, 1, 0, 0},
						   {0, 0, 1, 0}, {0, 0, 0, 1}}};

struct Model{
	ObjModel model;
	FPos3 worldPos;
	Mat4x4 transform = Identity4x4;
};

struct Camera{
	FPos3 position={0, 0, 0};
	Mat4x4 transform = Identity4x4;
};

struct Scene{
	Screen screen;
	Viewport port;

	std::vector<Model> models;
	uint32_t bgColor = Color(45, 45, 45, 255);
};
