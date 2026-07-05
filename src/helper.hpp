#pragma once
#include "types.hpp"
#include <cmath>
#include <cstdlib>

void changePixel(Screen& screen, Pos2 pos, uint32_t color);

void drawLine(Screen& screen, Pos2 P0, Pos2 P1, uint32_t color=WHITE);

std::vector<short int> interpolatePoints(short int i0, short int o0, short int i1, short int o1);

void edgeInterpolate(short int l0, short int i0, short int l1, short int i1,
		short int l2, short int i2, std::vector<short int>& v02, std::vector<short int>& v012);

std::vector<float> interpolatePointsFloat(float l0, float i0, float l1, float i1);

void edgeInterpolateFloat(short int l0, float i0, short int l1, float i1,
		short int l2, float i2, std::vector<float>& v02, std::vector<float>& v012);

void drawWireframeTriangle(Screen& screen, Pos2 p0, Pos2 p1, Pos2 p2, uint32_t color=WHITE);

void drawFilledTriangle(Screen& screen, Pos2 p0, Pos2 p1, Pos2 p2, uint32_t color,
		std::vector<float> zs);

void drawTexturedTriangle(Screen& screen, const Texture& tex, Pos2 p0, Pos2 p1, Pos2 p2,
                           std::vector<float> zs, std::vector<float> us, std::vector<float> vs);

void drawShadedTriangle(Screen& screen, Pos2 p0, float h0, Pos2 p1, float h1,
		Pos2 p2, float h2, uint32_t color=WHITE);

FPos2 worldToViewport(Viewport& port, FPos3 pos);

Pos2 viewportToCanvas(Screen& screen, Viewport& port, FPos2 pos);

Pos2 projectVertex(Screen& screen, Viewport& port, Vec4 v);

float toRadians(float angle);

float PlaneToPointSignedDistance(Plane plane, FPos3 point);

std::vector<Vertex> TriToF3(std::vector<int> tri, std::vector<int> texTri, Model& model, Mat4x4 transform);

Mat4x4 makeRotationY(float degrees);

Mat4x4 makeTranslation(FPos3 pos);

Mat4x4 transpose(Mat4x4 m);

Mat4x4 multiply(Mat4x4 a, Mat4x4 b);

Vec4 multiplyVec4(Mat4x4 mat, Vec4 vec);

Clip ClipModelPlane(Model& model, Camera& cam,Mat4x4 transform);

std::vector<FPos3> ClipPolygonPlane(std::vector<FPos3> poly, Plane plane);

void renderModel(Screen& screen, Camera& cam, Model& model,
		Mat4x4 transform=Identity4x4);

void renderScene(Scene& scene, Camera& cam);
