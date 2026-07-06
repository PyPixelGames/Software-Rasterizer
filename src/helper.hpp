#pragma once
#include "types.hpp"
#include <cmath>
#include <cstdlib>

void changePixel(Screen& screen, Pos2 pos, uint32_t color);

void drawLine(Screen& screen, Pos2 P0, Pos2 P1, uint32_t color=WHITE);

std::vector<short int> interpolatePoints(short int i0, short int o0, short int i1, short int o1);

template<typename T, size_t N>
void edgeInterpolateStatic(short int l0, T i0, short int l1, T i1, short int l2, T i2,
		std::array<T, N>& v02, std::array<T, N>& v012, short int baseOffset);

template <typename... ShaderFns>
void drawTriangle(Screen& screen, RasterTriangle tri,int MinY, int MaxY,
		ShaderFns&&... shaders);

FPos2 worldToViewport(Viewport& port, FPos3 pos);

Pos2 viewportToCanvas(Screen& screen, Viewport& port, FPos2 pos);

Pos2 projectVertex(Screen& screen, Viewport& port, Vec4 v);

float toRadians(float angle);

float PlaneToPointSignedDistance(Plane plane, FPos3 point);

std::vector<Vertex> DataToVertex(std::vector<int> tri, std::vector<int> texTri,
		Model& model, Mat4x4 transform);

Mat4x4 makeRotationY(float degrees);

Mat4x4 makeTranslation(FPos3 pos);

Mat4x4 transpose(Mat4x4 m);

Mat4x4 multiply(Mat4x4 a, Mat4x4 b);

Vec4 multiplyVec4(Mat4x4 mat, Vec4 vec);

Clip ClipModelPlane(Model& model, Camera& cam, Mat4x4 transform);

std::vector<Vertex> ClipPolygonPlane(std::vector<Vertex> poly, Plane plane);

void projectModel(Screen& screen, std::vector<RasterTriangle>& out, Camera& cam, Model& model,
		Mat4x4 transform, RasterPool& pool, const std::vector<Light>& lights);

void accumulateLighting(FPos3 n, FPos3 worldPos, const std::vector<Light>& lights,
                                float& outR, float& outG, float& outB);

void rasterizeBand(Screen& screen, const std::vector<RasterTriangle>& jobs,
                    int yStart, int yEnd);

void rasterizeParallel(Screen& screen, const std::vector<RasterTriangle>& jobs, RasterPool& pool);

void renderScene(Scene& scene, Camera& cam, RasterPool& pool);
