#include "helper.hpp"
#include <iostream>
#include <algorithm>

inline Vec4 F3ToVec4(FPos3 p){
	return {p.x, p.y, p.z, 1};
}

inline FPos3 cross(FPos3 a, FPos3 b){
	return { a.y*b.z - a.z*b.y,
	         a.z*b.x - a.x*b.z,
	         a.x*b.y - a.y*b.x };
}

inline float dot(FPos3 a, FPos3 b){
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline Vertex lerp(Vertex a, Vertex b, float t){
    return {
        {a.pos.x+t*(b.pos.x-a.pos.x), a.pos.y+t*(b.pos.y-a.pos.y), a.pos.z+t*(b.pos.z-a.pos.z)},
        {a.uv.u+t*(b.uv.u-a.uv.u),   a.uv.v+t*(b.uv.v-a.uv.v)}
    };
}

inline bool isLineTriangle(Pos2 a, Pos2 b, Pos2 c){
    int minX = std::min({a.x, b.x, c.x});
    int maxX = std::max({a.x, b.x, c.x});
    int minY = std::min({a.y, b.y, c.y});
    int maxY = std::max({a.y, b.y, c.y});

    int w = maxX - minX;
    int h = maxY - minY;

    // If it has no width or no height, it can only render as a line/point.
    return (w <= 0 || h <= 0);
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
		values.push_back(static_cast<short int>(std::round(i)));
		i = i+a;
	}
	return values;
}

void edgeInterpolate(short int l0, short int i0, short int l1, short int i1,
		short int l2, short int i2, std::vector<short int>& v02, std::vector<short int>& v012){
	std::vector<short int> t01 = interpolatePoints(l0, i0, l1, i1);
	std::vector<short int> t12 = interpolatePoints(l1, i1, l2, i2);
	std::vector<short int> t02 = interpolatePoints(l0, i0, l2, i2);
	t01.pop_back();
	v012 = t01;
	v012.insert(v012.end(), t12.begin(), t12.end());
	v02 = t02;
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

void edgeInterpolateFloat(short int l0, float i0, short int l1, float i1,
		short int l2, float i2, std::vector<float>& v02, std::vector<float>& v012){
	std::vector<float> t01 = interpolatePointsFloat(l0, i0, l1, i1);
	std::vector<float> t12 = interpolatePointsFloat(l1, i1, l2, i2);
	std::vector<float> t02 = interpolatePointsFloat(l0, i0, l2, i2);
	t01.pop_back();
	v012 = t01;
	v012.insert(v012.end(), t12.begin(), t12.end());
	v02 = t02;
}

void drawWireframeTriangle(Screen& screen, Pos2 p0, Pos2 p1, Pos2 p2, uint32_t color){
	drawLine(screen, p0, p1, color);
	drawLine(screen, p0, p2, color);
	drawLine(screen, p2, p1, color);
}

void drawFilledTriangle(Screen& screen, Pos2 p0, Pos2 p1, Pos2 p2, uint32_t color,
                        std::vector<float> zs){
    if (p1.y < p0.y) { std::swap(p1, p0); std::swap(zs[1], zs[0]); }
    if (p2.y < p0.y) { std::swap(p2, p0); std::swap(zs[2], zs[0]); }
    if (p2.y < p1.y) { std::swap(p2, p1); std::swap(zs[2], zs[1]); }

    std::vector<short int> x02, x012;
    std::vector<float> iz02, iz012;
    edgeInterpolate(p0.y, p0.x, p1.y, p1.x, p2.y, p2.x, x02, x012);
    edgeInterpolateFloat(p0.y, zs[0], p1.y, zs[1], p2.y, zs[2], iz02, iz012);

    std::vector<short int> *x_left, *x_right;
    std::vector<float> *iz_left, *iz_right;

    int crossProduct = (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);

    if (crossProduct < 0) {
        x_right = &x02;   x_left = &x012;
        iz_right = &iz02; iz_left = &iz012;
    } else {
        x_right = &x012;  x_left = &x02;
        iz_right = &iz012; iz_left = &iz02;
    }

    int yStart = std::max((int)p0.y, 0);
    int yEnd   = std::min((int)p2.y, screen.height - 1);

    for (int y = yStart; y <= yEnd; y++){
        int y_idx = y - p0.y;

        if (y_idx < 0 || y_idx >= static_cast<int>(x_left->size())) continue;

        int rawL = (*x_left)[y_idx];
        int rawR = (*x_right)[y_idx];
        if (rawL > rawR) continue;

        float zL = (*iz_left)[y_idx];
        float zR = (*iz_right)[y_idx];
        float dz = (rawR != rawL) ? (zR - zL) / (rawR - rawL) : 0.0f;

        int xL = std::max(rawL, 0);
        int xR = std::min(rawR, screen.width - 1);
        if (xL > xR) continue;

        float z = zL + dz * (xL - rawL);
        float* depthRow = screen.depthBuffer.data() + y * screen.width;
        uint32_t* pixRow = screen.pixelBuffer.data() + y * screen.width;

        for (int x = xL; x <= xR; x++, z += dz){
            if (z >= depthRow[x]){
                pixRow[x] = color;
                depthRow[x] = z;
            }
        }
    }
}

void drawTexturedTriangle(Screen& screen, const Texture& tex, Pos2 p0, Pos2 p1, Pos2 p2,
                           std::vector<float> zs, std::vector<float> us, std::vector<float> vs){
    if (p1.y < p0.y) { std::swap(p1, p0); std::swap(zs[1], zs[0]); std::swap(us[1], us[0]); std::swap(vs[1], vs[0]); }
    if (p2.y < p0.y) { std::swap(p2, p0); std::swap(zs[2], zs[0]); std::swap(us[2], us[0]); std::swap(vs[2], vs[0]); }
    if (p2.y < p1.y) { std::swap(p2, p1); std::swap(zs[2], zs[1]); std::swap(us[2], us[1]); std::swap(vs[2], vs[1]); }

    std::vector<short int> x02, x012;
    std::vector<float> iz02, iz012, iu02, iu012, iv02, iv012;
    edgeInterpolate(p0.y, p0.x, p1.y, p1.x, p2.y, p2.x, x02, x012);
    edgeInterpolateFloat(p0.y, zs[0], p1.y, zs[1], p2.y, zs[2], iz02, iz012);
    edgeInterpolateFloat(p0.y, us[0], p1.y, us[1], p2.y, us[2], iu02, iu012);
    edgeInterpolateFloat(p0.y, vs[0], p1.y, vs[1], p2.y, vs[2], iv02, iv012);

    std::vector<short int> *x_left, *x_right;
    std::vector<float> *iz_left, *iz_right, *iu_left, *iu_right, *iv_left, *iv_right;

    int crossProduct = (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);

    if (crossProduct < 0) {
        x_right = &x02;   x_left = &x012;
        iz_right = &iz02; iz_left = &iz012;
        iu_right = &iu02; iu_left = &iu012;
        iv_right = &iv02; iv_left = &iv012;
    } else {
        x_right = &x012;  x_left = &x02;
        iz_right = &iz012; iz_left = &iz02;
        iu_right = &iu012; iu_left = &iu02;
        iv_right = &iv012; iv_left = &iv02;
    }

    int yStart = std::max((int)p0.y, 0);
    int yEnd   = std::min((int)p2.y, screen.height - 1);

    for (int y = yStart; y <= yEnd; y++){
        int y_idx = y - p0.y;
        if (y_idx < 0 || y_idx >= static_cast<int>(x_left->size())) continue;

        int rawL = (*x_left)[y_idx];
        int rawR = (*x_right)[y_idx];
        if (rawL > rawR) continue;

        float zL = (*iz_left)[y_idx], zR = (*iz_right)[y_idx];
        float uL = (*iu_left)[y_idx], uR = (*iu_right)[y_idx];
        float vL = (*iv_left)[y_idx], vR = (*iv_right)[y_idx];

        float dz = (rawR != rawL) ? (zR - zL) / (rawR - rawL) : 0.0f;
        float du = (rawR != rawL) ? (uR - uL) / (rawR - rawL) : 0.0f;
        float dv = (rawR != rawL) ? (vR - vL) / (rawR - rawL) : 0.0f;

        int xL = std::max(rawL, 0);
        int xR = std::min(rawR, screen.width - 1);
        if (xL > xR) continue;

        float z = zL + dz * (xL - rawL);
        float u = uL + du * (xL - rawL);
        float v = vL + dv * (xL - rawL);

        float* depthRow = screen.depthBuffer.data() + y * screen.width;
        uint32_t* pixRow = screen.pixelBuffer.data() + y * screen.width;

        for (int x = xL; x <= xR; x++, z += dz, u += du, v += dv){
            if (z >= depthRow[x]){
                float invZ = z; // 1/z
                float realU = u / invZ;
                float realV = v / invZ;
                pixRow[x] = tex.sample(realU, realV);
                depthRow[x] = z;
            }
        }
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

std::vector<Vertex> TriToF3(std::vector<int> tri, std::vector<int> texTri, Model& model, Mat4x4 transform){
    std::vector<Vertex> tris;
    for (size_t i = 0; i < tri.size(); i++){
        int v  = tri[i];
        int vt = texTri[i];

        Vec4 vert = multiplyVec4(transform, {
            model.model.vertices[v-1].x,
            model.model.vertices[v-1].y,
            model.model.vertices[v-1].z, 1});

        TextureCoord c = (vt > 0) ? model.model.texCoords[vt-1] : TextureCoord{0.0f, 0.0f};

        tris.push_back(Vertex{ FPos3{vert.x, vert.y, vert.z}, c });
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

std::vector<Vertex> ClipPolygonPlane(std::vector<Vertex> poly, Plane plane){
    std::vector<Vertex> out;
    int n = poly.size();
    for (int i = 0; i < n; i++){
        Vertex cur = poly[i];
        Vertex nxt = poly[(i+1) % n];

        float dCur = PlaneToPointSignedDistance(plane, cur.pos);
        float dNxt = PlaneToPointSignedDistance(plane, nxt.pos);

        if (dCur >= 0) out.push_back(cur);

        if ((dCur >= 0) != (dNxt >= 0)){
            float t = dCur / (dCur - dNxt);
            out.push_back(lerp(cur, nxt, t));
        }
    }
    return out;
}

void renderModel(Screen& screen, Camera& cam, Model& model, Mat4x4 transform){
	Clip clip = ClipModelPlane(model, cam, transform);
	if (clip.state==ClipState::Outside){
		return;
	}

	for (size_t t = 0; t < model.model.tris.size(); t++){
		std::vector<int> tri = model.model.tris[t];
		std::vector<int> texTri = model.model.triTexCoords[t];
		std::vector<Vertex> poly = TriToF3(tri, texTri, model, transform);

		if (clip.state==ClipState::Intersect){
			for (int idx : clip.intersectedIndexes){
				poly = ClipPolygonPlane(poly, cam.planes[idx]);
				if (poly.empty()) break;
			}
		}

		if (poly.size() >= 3){
			FPos3 e1{poly[1].pos.x-poly[0].pos.x, poly[1].pos.y-poly[0].pos.y, poly[1].pos.z-poly[0].pos.z};
			FPos3 e2{poly[2].pos.x-poly[0].pos.x, poly[2].pos.y-poly[0].pos.y, poly[2].pos.z-poly[0].pos.z};
			FPos3 normal = cross(e1, e2);
			FPos3 viewDir = poly[0].pos;

			if (dot(normal, viewDir) >= 0.0f){
				continue; //Back-face culling
			}

			// fan-triangulate: (poly[0], poly[i], poly[i+1]) for i in 1..poly.size()-2
			for (size_t i = 1; i+1 < poly.size(); i++){
				Pos2 p0 = projectVertex(screen, cam.port, F3ToVec4(poly[0].pos));
				Pos2 p1 = projectVertex(screen, cam.port, F3ToVec4(poly[i].pos));
				Pos2 p2 = projectVertex(screen, cam.port, F3ToVec4(poly[i+1].pos));

				if (p0.y == p1.y && p1.y == p2.y) continue; // Fully flat triangle

				if (isLineTriangle(p0, p1, p2)) continue; // A line, not a triangle

				//drawFilledTriangle(screen, p0, p1, p2,
				//		Colors[tri[0]%std::size(Colors)],
				//		{1.0f/poly[0].pos.z, 1.0f/poly[i].pos.z, 1.0f/poly[i+1].pos.z});
				float iz0 = 1.0f/poly[0].pos.z;
				float izI = 1.0f/poly[i].pos.z;
				float izI1 = 1.0f/poly[i+1].pos.z;

				drawTexturedTriangle(screen, model.texture, p0, p1, p2,
						{iz0, izI, izI1},
						{poly[0].uv.u*iz0, poly[i].uv.u*izI, poly[i+1].uv.u*izI1},
						{poly[0].uv.v*iz0, poly[i].uv.v*izI, poly[i+1].uv.v*izI1});
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
