#include "helper.hpp"
#include <iostream>

inline Vec4 F3ToVec4(FPos3 p){
	return {p.x, p.y, p.z, 1};
}

void changePixel(Screen& screen, Pos2 pos, uint32_t color){
	if (pos.x < 0 || pos.x >= screen.width ||
        pos.y < 0 || pos.y >= screen.height) return;
	screen.pixelBuffer[pos.y*screen.width+pos.x]=color;
}

void drawLine(Screen& screen, Pos2 P0, Pos2 P1, uint32_t color){
	if (abs(P1.x-P0.x)>abs(P1.y-P0.y)){
		//line is more horizontal
		if (P0.x > P1.x){
			std::swap(P0, P1);
		}

		std::vector<short int> ys = interpolatePoints(P0.x, P0.y, P1.x, P1.y);
		for (short int x=P0.x; x<=P1.x; x++){
			changePixel(screen, Pos2{x, ys[x-P0.x]}, color);
		}
	}else{
		//line is more vertical
		if (P0.y > P1.y){
			std::swap(P0, P1);
		}

		std::vector<short int> xs = interpolatePoints(P0.y, P0.x, P1.y, P1.x);
		for (short int y=P0.y; y<=P1.y; y++){
			changePixel(screen, Pos2{xs[y-P0.y], y}, color);
		}
	}
}

std::vector<short int> interpolatePoints(short int l0, short int i0, short int l1, short int i1){
	// the "i" variables are the ones you don't want to interpolate
	// "l" is for leave me alone
	if (l0==l1){
		return {i0};
	}

	std::vector<short int> values;
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

void drawWireframeTriangle(Screen& screen, Pos2 p0, Pos2 p1, Pos2 p2, uint32_t color){
	drawLine(screen, p0, p1, color);
	drawLine(screen, p0, p2, color);
	drawLine(screen, p2, p1, color);
}

void drawFilledTriangle(Screen& screen, Pos2 p0, Pos2 p1, Pos2 p2, uint32_t color){
	if (p1.y<p0.y) std::swap(p1, p0);
	if (p2.y<p0.y) std::swap(p2, p0);
	if (p2.y<p1.y) std::swap(p2, p1);

	std::vector<short int> x01 = interpolatePoints(p0.y, p0.x, p1.y, p1.x);
	std::vector<short int> x12 = interpolatePoints(p1.y, p1.x, p2.y, p2.x);
	std::vector<short int> x02 = interpolatePoints(p0.y, p0.x, p2.y, p2.x);

	x01.pop_back();
	std::vector<short int> x012 = x01;
	x012.insert(x012.end(), x12.begin(), x12.end());

	std::vector<short int>* x_left;
	std::vector<short int>* x_right;
	int m = std::floor(x02.size()/2);
	if (x02[m] > x012[m]){
		x_right = &x02;
		x_left = &x012;
	}else{
		x_right = &x012;
		x_left = &x02;
	}

	int yStart = std::max((int)p0.y, 0);
	int yEnd = std::min((int)p2.y, screen.height - 1);

	for (int y=yStart; y<=yEnd; y++){
		int idx = y - p0.y;
		int xL = std::max((int)(*x_left)[idx], 0);
		int xR = std::min((int)(*x_right)[idx], screen.width - 1);
		if (xL > xR) continue;

		uint32_t* row = screen.pixelBuffer.data() + y * screen.width;
		std::fill(row + xL, row + xR + 1, color);
	}
}

void drawShadedTriangle(Screen& screen, Pos2 p0, float h0, Pos2 p1, float h1,
		Pos2 p2, float h2, uint32_t color){
	if (p1.y<p0.y) std::swap(p1, p0);
	if (p2.y<p0.y) std::swap(p2, p0);
	if (p2.y<p1.y) std::swap(p2, p1);

	std::vector<short int> x01 = interpolatePoints(p0.y, p0.x, p1.y, p1.x);
	std::vector<short int> x12 = interpolatePoints(p1.y, p1.x, p2.y, p2.x);
	std::vector<short int> x02 = interpolatePoints(p0.y, p0.x, p2.y, p2.x);

	x01.pop_back();
	std::vector<short int> x012 = x01;
	x012.insert(x012.end(), x12.begin(), x12.end());

	std::vector<float> h01 = interpolatePointsFloat(p0.y, h0, p1.y, h1);
	std::vector<float> h12 = interpolatePointsFloat(p1.y, h1, p2.y, h2);
	std::vector<float> h02 = interpolatePointsFloat(p0.y, h0, p2.y, h2);

	h01.pop_back();
	std::vector<float> h012 = h01;
	h012.insert(h012.end(), h12.begin(), h12.end());

	std::vector<short int>* x_left;
	std::vector<short int>* x_right;
	std::vector<float>* h_left;
	std::vector<float>* h_right;

	int m = std::floor(x02.size()/2);
	if (x02[m] > x012[m]){
		x_right = &x02;  h_right = &h02;
		x_left = &x012;  h_left = &h012;
	}else{
		x_right = &x012; h_right = &h012;
		x_left = &x02;   h_left = &h02;
	}

	int r, g, b, a;
	GetColor(color, r, g, b, a);
	const float rf = static_cast<float>(r);
	const float gf = static_cast<float>(g);
	const float bf = static_cast<float>(b);

	int yStart = std::max((int)p0.y, 0);
	int yEnd   = std::min((int)p2.y, screen.height - 1);

	for (int y=yStart; y<=yEnd; y++){
		int y_idx = y - p0.y;

		int rawL = (*x_left)[y_idx];
		int rawR = (*x_right)[y_idx];
		if (rawL > rawR) continue;

		std::vector<float> h_segments = interpolatePointsFloat(
				static_cast<float>(rawL), (*h_left)[y_idx],
				static_cast<float>(rawR), (*h_right)[y_idx]);

		int xL = std::max(rawL, 0);
		int xR = std::min(rawR, screen.width - 1);
		if (xL > xR) continue;

		uint32_t* row = screen.pixelBuffer.data() + y * screen.width;

		for (int x=xL; x<=xR; x++){
			int x_idx = x - rawL;
			float h = h_segments[x_idx];
			row[x] = Color(
				static_cast<int>(rf*h),
				static_cast<int>(gf*h),
				static_cast<int>(bf*h),
				255
			);
		}
	}
}

FPos2 worldToViewport(Viewport& port, FPos3 pos){
	return FPos2{(pos.x*port.distance)/pos.z, (pos.y*port.distance)/pos.z};
}

Pos2 viewportToCanvas(Screen& screen, Viewport& port, FPos2 pos){
	// We have to add half the width and half the height to our calculation
	// because the screen's (0, 0) is not in the center of the screen.

    return Pos2{
        static_cast<short int>(
				std::round((pos.x * screen.width /port.width)
					+ static_cast<float>(screen.width) / 2)),
        static_cast<short int>(
				std::round((-pos.y * screen.height / port.height)
				+ static_cast<float>(screen.height) / 2))
    };
}

Pos2 projectVertex(Screen& screen, Viewport& port, Vec4 v){
	return viewportToCanvas(screen, port, worldToViewport(port,
				{v.x,v.y,v.z}));
}

float toRadians(float angle){
	return angle * (M_PI / 180.0f);
}

float PlaneToPointSignedDistance(Plane plane, FPos3 point){
	return
		(point.x*plane.normal.x)+
		(point.y*plane.normal.y)+
		(point.z*plane.normal.z)+
		plane.D;
}

std::vector<FPos3> TriToF3(std::vector<int> tri, Model& model, Mat4x4 transform){
	std::vector<FPos3> tris;
	for (int v : tri){
        Vec4 vert = multiplyVec4(transform, {
            model.model.vertices[v-1].x,
            model.model.vertices[v-1].y,
            model.model.vertices[v-1].z, 1});
        FPos3 p{vert.x, vert.y, vert.z};
        tris.push_back(p);
    }
	return tris;
}

Mat4x4 makeRotationY(float degrees) {
    float a = toRadians(degrees);
    float s = std::sin(a);
    float c = std::cos(a);
    return {{
        { c,  0, s, 0},
        { 0,  1, 0, 0},
        {-s,  0, c, 0},
        { 0,  0, 0, 1}
    }};
}

Mat4x4 makeTranslation(FPos3 pos){
    Mat4x4 m = Identity4x4;
    m[0][3] = pos.x;
    m[1][3] = pos.y;
    m[2][3] = pos.z;
    return m;
}

Mat4x4 transpose(Mat4x4 m) {
    Mat4x4 result{};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            result[i][j] = m[j][i];
    return result;
}

Mat4x4 multiply(Mat4x4 a, Mat4x4 b) {
    Mat4x4 result{};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                result[i][j] += a[i][k] * b[k][j];
    return result;
}

Vec4 multiplyVec4(Mat4x4 mat, Vec4 vec){
	return {
        mat[0][0]*vec.x + mat[0][1]*vec.y +
				mat[0][2]*vec.z + mat[0][3]*vec.w,
        mat[1][0]*vec.x + mat[1][1]*vec.y +
				mat[1][2]*vec.z + mat[1][3]*vec.w,
        mat[2][0]*vec.x + mat[2][1]*vec.y +
				mat[2][2]*vec.z + mat[2][3]*vec.w,
        mat[3][0]*vec.x + mat[3][1]*vec.y +
				mat[3][2]*vec.z + mat[3][3]*vec.w,
    };
}

Clip ClipModelPlane(Model& model, Camera& cam, Mat4x4 transform){
	Vec4 centerV = multiplyVec4(transform, {
			model.model.sphere_center.x,
			model.model.sphere_center.y,
			model.model.sphere_center.z,
			1});
	FPos3 worldCenter{centerV.x, centerV.y, centerV.z};

	Clip clip;
	for (int i=0; i<std::size(cam.planes); i++){
		Plane plane = cam.planes[i];
		float d = PlaneToPointSignedDistance(plane, worldCenter);
		if (d > model.model.sphere_radius){
			continue;
		}else if (d < -model.model.sphere_radius){
			clip.state = ClipState::Outside;
			break;
		}else{
			clip.state = ClipState::Intersect;
			clip.intersectedIndexes.push_back(i);
		}
	}

	return clip;
}

std::vector<FPos3> ClipPolygonPlane(std::vector<FPos3> poly, Plane plane){
    std::vector<FPos3> out;
    int n = poly.size();
    for (int i = 0; i < n; i++){
        FPos3 cur = poly[i], nxt = poly[(i+1)%n];
        float dCur = PlaneToPointSignedDistance(plane, cur);
        float dNxt = PlaneToPointSignedDistance(plane, nxt);
        if (dCur >= 0) out.push_back(cur);
        if ((dCur >= 0) != (dNxt >= 0)){
            float t = dCur / (dCur - dNxt);
            out.push_back(FPos3{
                cur.x + t*(nxt.x-cur.x),
                cur.y + t*(nxt.y-cur.y),
                cur.z + t*(nxt.z-cur.z)});
        }
    }
    return out;
}

void renderModel(Screen& screen, Camera& cam, Model& model, Mat4x4 transform){
	Clip clip = ClipModelPlane(model, cam, transform);
	if (clip.state==ClipState::Outside){
		return;
	}

	for (std::vector<int> tri : model.model.tris){
		std::vector<FPos3> poly = TriToF3(tri, model, transform);

		if (clip.state==ClipState::Intersect){
			for (int idx : clip.intersectedIndexes){
				poly = ClipPolygonPlane(poly, cam.planes[idx]);
				if (poly.empty()) break;
			}
		}

		if (poly.size() >= 3){
			// fan-triangulate: (poly[0], poly[i], poly[i+1]) for i in 1..poly.size()-2
			for (size_t i = 1; i+1 < poly.size(); i++){
				drawWireframeTriangle(screen,
						projectVertex(screen, cam.port, F3ToVec4(poly[0])),
						projectVertex(screen, cam.port, F3ToVec4(poly[i])),
						projectVertex(screen, cam.port, F3ToVec4(poly[i+1])));

				//drawShadedTriangle(screen,
						//projectVertex(screen, cam.port, F3ToVec4(poly[0])), 0.0f,
						//projectVertex(screen, cam.port, F3ToVec4(poly[i])), 1.0f,
						//projectVertex(screen, cam.port, F3ToVec4(poly[i+1])), 0.5f,
						//GREEN);
			}
		}
	}
}

void renderScene(Scene& scene, Camera& cam){
	scene.screen.clear(scene.bgColor);

	for (auto model: scene.models){
		Mat4x4 translation = makeTranslation(model.worldPos);
        Mat4x4 combined = multiply(translation, model.transform);
		renderModel(scene.screen, cam, model, combined);
	}
}
