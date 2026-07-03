#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <array>
#include <cmath>
#include <climits>

inline uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255){
    return (a << 24) | (r << 16) | (g << 8) | b;
}

inline uint32_t BLACK = 0xFF000000u;
inline uint32_t WHITE = 0xFFFFFFFFu;
inline uint32_t GREEN = 0xFF00FF00u;
inline uint32_t BLUE = 0xFF0000FFu;
inline uint32_t RED = 0xFFFF0000u;
inline uint32_t LIGHT_GREY   = Color(220, 220, 220);
inline uint32_t GREY         = Color(128, 128, 128);
inline uint32_t DARK_GREY    = Color(220, 220, 220);
inline uint32_t PINK         = Color(225, 96, 208);
inline uint32_t PURPLE       = Color(160, 32, 255);
inline uint32_t LIGHT_BLUE   = Color(80, 208, 225);
inline uint32_t YELLOW_GREEN = Color(96, 225, 128);
inline uint32_t YELLOW       = Color(225, 224, 32);
inline uint32_t ORANGE       = Color(225, 160, 16);
inline uint32_t BROWN        = Color(140, 108, 76);
inline uint32_t PALE         = Color(225, 208, 160);

inline std::vector<uint32_t> Colors = {
	GREEN,
	BLUE,
	RED,
	PINK,
	PURPLE,
	LIGHT_BLUE,
	YELLOW,
	BROWN,
	PALE,
	YELLOW_GREEN,
	ORANGE
};

inline void GetColor(uint32_t color, int& r, int& g, int& b, int& a) {
    r = (color >> 16) & 0xFF;
    g = (color >> 8)  & 0xFF;
    b = (color)       & 0xFF;
    a = (color >> 24) & 0xFF;
}

enum ClipState{
	Outside=0,
	Intersect=1,
	Inside=2,
};

struct Clip{
	ClipState state = ClipState::Inside;
	std::vector<int> intersectedIndexes;
};

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
	std::vector<float> depthBuffer = std::vector<float>(width*height, 0.0f);

	void clear(uint32_t color = 0xFF000000U){
        std::fill(pixelBuffer.begin(), pixelBuffer.end(), color);
        std::fill(depthBuffer.begin(), depthBuffer.end(), 0.0f);
    }
};

struct Viewport{
    float fovDegrees = 60.0f;
    float distance = 1.0f;
    float width;
    float height;

    Viewport(float fov = 60.0f, float aspect = 800.0f/600.0f)
        : fovDegrees(fov)
    {
        float halfFovRad = (fovDegrees / 2.0f) * (M_PI / 180.0f);
        width  = 2.0f * distance * std::tan(halfFovRad);
        height = width / aspect;
    }
};

struct TextureCoord { float u, v; };

struct ObjModel{
    std::string objName;
    std::vector<FPos3> vertices;
    std::vector<TextureCoord> texCoords;
    std::vector<FPos3> normals;
    std::vector<std::vector<int>> tris;
	float sphere_radius;
	FPos3 sphere_center;
};

using Mat4x4 = std::array<std::array<float, 4>, 4>;
constexpr Mat4x4 Identity4x4 = Mat4x4{{{1, 0, 0, 0}, {0, 1, 0, 0},
						   {0, 0, 1, 0}, {0, 0, 0, 1}}};
struct Model{
	ObjModel model;
	FPos3 worldPos;
	Mat4x4 transform = Identity4x4;
};

struct Plane{
	FPos3 normal;
	float D;
};

struct Camera{
    FPos3 position={0, 0, 0};
    Viewport port;
    Mat4x4 transform = Identity4x4;

    Plane near, left, right, bottom, top;
    std::vector<Plane> planes;
	float nearClip = 0.1f;

    Camera(float fovDegrees = 90.0f, float aspect = 800.0f/600.0f) : port(fovDegrees, aspect){
        float halfW = port.width / 2.0f;
        float halfH = port.height / 2.0f;
        float d = port.distance;

        float leftLen = std::sqrt(d*d + halfW*halfW);
        float topLen  = std::sqrt(d*d + halfH*halfH);

        near   = Plane{{0, 0, 1}, -nearClip};
        left   = Plane{{ d/leftLen, 0, halfW/leftLen}, 0};
        right  = Plane{{-d/leftLen, 0, halfW/leftLen}, 0};
        bottom = Plane{{0,  d/topLen, halfH/topLen}, 0};
        top    = Plane{{0, -d/topLen, halfH/topLen}, 0};

        planes = {near, left, right, bottom, top};
    }
};

struct Scene{
	Screen screen;

	std::vector<Model> models;
	uint32_t bgColor = Color(45, 45, 45, 255);
};
