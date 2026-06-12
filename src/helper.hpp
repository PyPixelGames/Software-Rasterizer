#pragma once
#include "types.hpp"
#include <cmath>

void changePixel(Screen& screen, Pos2 pos, uint32_t color);

void drawLine(Screen& screen, Pos2 P0, Pos2 P1, uint32_t color=WHITE);

std::vector<int> interpolatePoints(int i0, int o0, int i1, int o1);

void drawWireframeTriangle(Screen& screen, Pos2 p0, Pos2 p1, Pos2 p2);

void drawFilledTriangle(Screen& screen, Pos2 p0, Pos2 p1, Pos2 p2, uint32_t color=WHITE);
