#include "helper.hpp"
#include <iostream>

void changePixel(Screen& screen, Pos2 pos, uint32_t color){
	screen.pixelBuffer[pos.y*screen.width+pos.x]=color;
}

void drawLine(Screen& screen, Pos2 P0, Pos2 P1, uint32_t color){
	if (abs(P1.x-P0.x)>abs(P1.y-P0.y)){
		//line is more horizontal
		if (P0.x > P1.x){
			std::swap(P0, P1);
		}

		std::vector<int> ys = interpolatePoints(P0.x, P0.y, P1.x, P1.y);
		for (int x=P0.x; x<=P1.x; x++){
			changePixel(screen, Pos2{x, ys[x-P0.x]}, color);
		}
	}else{
		//line is more vertical
		if (P0.y > P1.y){
			std::swap(P0, P1);
		}

		std::vector<int> xs = interpolatePoints(P0.y, P0.x, P1.y, P1.x);
		for (int y=P0.y; y<=P1.y; y++){
			changePixel(screen, Pos2{xs[y-P0.y], y}, color);
		}
	}
}

std::vector<int> interpolatePoints(int l0, int i0, int l1, int i1){
	// the "i" variables are the ones you don't want to interpolate
	// "l" is for leave me alone
	if (l0==l1){
		return {i0};
	}

	std::vector<int> values;
	values.reserve(l1-l0+1);
	float a = (float)(i1-i0)/(l1-l0);
	float i = i0;
	for (int l=l0; l<=l1; l++){
		values.push_back(static_cast<int>(i));
		i = i+a;
	}
	return values;
}

std::vector<float> interpolatePointsFloat(float l0, float i0, float l1, float i1){
	// the "i" variables are the ones you don't want to interpolate
	// "l" is for leave me alone
	if (l0==l1){
		return {i0};
	}

	std::vector<float> values;
	values.reserve(static_cast<int>(l1-l0+1));
	float a = (float)(i1-i0)/(l1-l0);
	float i = i0;
	for (int l=l0; l<=l1; l++){
		values.push_back(i);
		i = i+a;
	}
	return values;
}

void drawWireframeTriangle(Screen& screen, Pos2 p0, Pos2 p1, Pos2 p2){
	drawLine(screen, p0, p1);
	drawLine(screen, p0, p2);
	drawLine(screen, p2, p1);
}

void drawFilledTriangle(Screen& screen, Pos2 p0, Pos2 p1, Pos2 p2, uint32_t color){
	//rearange the points so p0 is the lowest and p2 is the highest
	if (p1.y<p0.y) std::swap(p1, p0);
	if (p2.y<p0.y) std::swap(p2, p0);
	if (p2.y<p1.y) std::swap(p2, p1);

	// x02 is the long side, x01 and x12 are the shorter sides
	std::vector<int> x01 = interpolatePoints(p0.y, p0.x, p1.y, p1.x);
	std::vector<int> x12 = interpolatePoints(p1.y, p1.x, p2.y, p2.x);
	std::vector<int> x02 = interpolatePoints(p0.y, p0.x, p2.y, p2.x);

	//combine the shorter sides. Don't forget to remove the mutual value
	x01.pop_back();
	std::vector<int> x012 = x01;
	x012.insert(x012.end(), x12.begin(), x12.end());

	// Figure out what side is left and what side is right
	std::vector<int> x_left;
	std::vector<int> x_right;
	int m = std::floor(x02.size()/2);
	if (x02[m] > x012[m]){
		x_right = x02;
		x_left = x012;
	}else{
		x_right = x012;
		x_left = x02;
	}

	//draw the horizonral lines (without using the drawLine since that one is very
	//general and has a lot of quards and work arounds to make any line work. Here we need
	//only horizontal lines, so it's faster to do it like that).
	for (int y=p0.y; y<=p2.y; y++){
		for (int x=x_left[y-p0.y]; x<=x_right[y-p0.y]; x++){
			changePixel(screen, Pos2{x, y}, color);
		}
	}
}

void drawShadedTriangle(Screen& screen, Pos2 p0, float h0, Pos2 p1, float h1,
		Pos2 p2, float h2, uint32_t color){
	// almost everything here is the same as drawFilledTriangle, so for more explanations look there
	if (p1.y<p0.y) std::swap(p1, p0);
	if (p2.y<p0.y) std::swap(p2, p0);
	if (p2.y<p1.y) std::swap(p2, p1);

	std::vector<int> x01 = interpolatePoints(p0.y, p0.x, p1.y, p1.x);
	std::vector<int> x12 = interpolatePoints(p1.y, p1.x, p2.y, p2.x);
	std::vector<int> x02 = interpolatePoints(p0.y, p0.x, p2.y, p2.x);

	x01.pop_back();
	std::vector<int> x012 = x01;
	x012.insert(x012.end(), x12.begin(), x12.end());


    std::vector<float> h01 = interpolatePointsFloat(p0.y, h0, p1.y, h1);
    std::vector<float> h12 = interpolatePointsFloat(p1.y, h1, p2.y, h2);
    std::vector<float> h02 = interpolatePointsFloat(p0.y, h0, p2.y, h2);

    h01.pop_back();
    std::vector<float> h012 = h01;
    h012.insert(h012.end(), h12.begin(), h12.end());

	std::vector<int>* x_left;
	std::vector<int>* x_right;

	std::vector<float>* h_left;
	std::vector<float>* h_right;

	int m = std::floor(x02.size()/2);
	if (x02[m] > x012[m]){
		x_right = &x02;
		h_right = &h02;

		x_left = &x012;
		h_left = &h012;
	}else{
		x_right = &x012;
		h_right = &h012;

		x_left = &x02;
		h_left = &h02;
	}

	// Get rgb of the original color
	int r, g, b, a;
	GetColor(color, r, g, b, a);

	for (int y=p0.y; y<=p2.y; y++){
		int y_idx = y-p0.y;
		// The goal is to find the value h that gives the shade with (red*h, green*h, blue*h)
		std::vector<float> h_segments = interpolatePointsFloat(
				static_cast<float>((*x_left)[y_idx]),
				(*h_left)[y_idx],
				static_cast<float>((*x_right)[y_idx]),
				(*h_right)[y_idx]);
		for (int x=(*x_left)[y_idx]; x<=(*x_right)[y_idx]; x++){
			int x_idx = x-(*x_left)[y_idx];
			float h = h_segments[x_idx];
			changePixel(screen, Pos2{x, y}, Color(r*h, g*h, b*h, 255));
		}
	}
}
