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

template<typename T, size_t N>
void edgeInterpolateStatic(short int l0, T i0, short int l1, T i1, short int l2, T i2,
                           std::array<T, N>& v02, std::array<T, N>& v012, short int baseOffset) {
    // Helper lambda to incrementally step-fill flat array sections
    auto fillSegment = [&](short int startL, T startI, short int endL, T endI, std::array<T, N>& target) {
        if (startL == endL) {
            int idx = startL - baseOffset;
            if (idx >= 0 && idx < static_cast<int>(N)) target[idx] = startI;
            return;
        }

        float invDelta = 1.0f / (endL - startL);
        // Always compute the step rate as a float
        float stepRate = static_cast<float>(endI - startI) * invDelta;
        // Keep the running accumulator as a float to prevent precision loss
        float currentVal = static_cast<float>(startI);

        for (int l = startL; l <= endL; ++l) {
            int idx = l - baseOffset;
            if (idx >= 0 && idx < static_cast<int>(N)) {
                // Cast back to T only when saving to the array
                target[idx] = static_cast<T>(currentVal);
            }
            currentVal += stepRate;
        }
    };

    // Fill structural long edge (0 to 2)
    fillSegment(l0, i0, l2, i2, v02);

    // Fill structural broken short edges (0 to 1, then 1 to 2)
    fillSegment(l0, i0, l1, i1, v012);
    fillSegment(l1, i1, l2, i2, v012);
}

template <typename... ShaderFns>
void drawTriangle(Screen& screen, RasterTriangle tri, int MinY, int MaxY, ShaderFns&&... shaders) {
    uint32_t color = PURPLE;

    // Sort vertices by Y coordinate
    if (tri.p1.y < tri.p0.y) {
        std::swap(tri.p1, tri.p0); std::swap(tri.z1, tri.z0);
        std::swap(tri.u1, tri.u0); std::swap(tri.v1, tri.v0);
    }
    if (tri.p2.y < tri.p0.y) {
        std::swap(tri.p2, tri.p0); std::swap(tri.z2, tri.z0);
        std::swap(tri.u2, tri.u0); std::swap(tri.v2, tri.v0);
    }
    if (tri.p2.y < tri.p1.y) {
        std::swap(tri.p2, tri.p1); std::swap(tri.z2, tri.z1);
        std::swap(tri.u2, tri.u1); std::swap(tri.v2, tri.v1);
    }

    // Early out if triangle is flat/degenerate or fully out of vertical bands
    if (tri.p2.y == tri.p0.y) return;
    int yStart = std::max((int)tri.p0.y, MinY);
    int yEnd   = std::min((int)tri.p2.y, MaxY);
    if (yStart > yEnd) return;

    // Compute localized vertical headroom index span
    int totalHeightSpan = tri.p2.y - tri.p0.y + 1;
    if (totalHeightSpan <= 0) return;

    // Use runtime stack arrays safely restricted to maximum possible screen limits
    constexpr size_t MAX_SPAN = 600;
    std::array<short int, MAX_SPAN> x02, x012;
    std::array<float, MAX_SPAN> iz02, iz012, iu02, iu012, iv02, iv012;

    short int baseOffset = tri.p0.y;

    // Calculate raster attributes over stack lines
    edgeInterpolateStatic(tri.p0.y, tri.p0.x, tri.p1.y, tri.p1.x, tri.p2.y, tri.p2.x, x02, x012, baseOffset);
    edgeInterpolateStatic(tri.p0.y, tri.z0,   tri.p1.y, tri.z1,   tri.p2.y, tri.z2,   iz02, iz012, baseOffset);
    edgeInterpolateStatic(tri.p0.y, tri.u0,   tri.p1.y, tri.u1,   tri.p2.y, tri.u2,   iu02, iu012, baseOffset);
    edgeInterpolateStatic(tri.p0.y, tri.v0,   tri.p1.y, tri.v1,   tri.p2.y, tri.v2,   iv02, iv012, baseOffset);

    // Explicitly identify left vs right bounding contours
    std::array<short int, MAX_SPAN> *x_left, *x_right;
    std::array<float, MAX_SPAN> *iz_left, *iz_right, *iu_left, *iu_right, *iv_left, *iv_right;

    int crossProduct = (tri.p1.x - tri.p0.x) * (tri.p2.y - tri.p0.y) - (tri.p1.y - tri.p0.y) * (tri.p2.x - tri.p0.x);

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

    // Scanline loops over target segment block bounds
    for (int y = yStart; y <= yEnd; y++) {
        int y_idx = y - baseOffset;
        if (y_idx < 0 || y_idx >= totalHeightSpan) continue;

        int rawL = (*x_left)[y_idx];
        int rawR = (*x_right)[y_idx];
        if (rawL > rawR) continue;

        float zL = (*iz_left)[y_idx], zR = (*iz_right)[y_idx];
        float uL = (*iu_left)[y_idx], uR = (*iu_right)[y_idx];
        float vL = (*iv_left)[y_idx], vR = (*iv_right)[y_idx];

        float invWidth = (rawR != rawL) ? 1.0f / (rawR - rawL) : 0.0f;
        float dz = (zR - zL) * invWidth;
        float du = (uR - uL) * invWidth;
        float dv = (vR - vL) * invWidth;

        int xL = std::max(rawL, 0);
        int xR = std::min(rawR, screen.width - 1);
        if (xL > xR) continue;

        float prestep = static_cast<float>(xL - rawL);
        float z = zL + dz * prestep;
        float u = uL + du * prestep;
        float v = vL + dv * prestep;

        float* depthRow = screen.depthBuffer.data() + y * screen.width;
        uint32_t* pixRow = screen.pixelBuffer.data() + y * screen.width;

        for (int x = xL; x <= xR; x++, z += dz, u += du, v += dv) {
            if (z >= depthRow[x]) {
                ShaderFragment frag{ {u, v, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, z, x, y };

                uint32_t outColor = color;
                ((outColor = shaders(frag, outColor)), ...);

                pixRow[x] = outColor;
                depthRow[x] = z;
            }
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

std::vector<Vertex> DataToVertex(std::vector<int> tri, std::vector<int> texTri, Model& model, Mat4x4 transform){
    std::vector<Vertex> tris;
    for (size_t i = 0; i < tri.size(); i++){
        int v  = tri[i];
        int vt = texTri[i];

        Vec4 vert = multiplyVec4(transform, {
            model.model.vertices[v-1].x,
            model.model.vertices[v-1].y,
            model.model.vertices[v-1].z, 1});

        TextureCoord c = (vt > 0) ? model.model.texCoords[vt-1] : TextureCoord{0.0f, 0.0f};

        tris.push_back(Vertex{ FPos3{vert.x, vert.y, vert.z}, c, {}});
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

void projectModel(Screen& screen, std::vector<RasterTriangle>& out, Camera& cam,
                           Model& model, Mat4x4 transform, RasterPool& pool) {
    Clip clip = ClipModelPlane(model, cam, transform);
    if (clip.state == ClipState::Outside) return;

    size_t totalTris = model.model.tris.size();

    // Create an independent storage array for each thread to avoid lock contention
    std::vector<std::vector<RasterTriangle>> perThreadOut(pool.numThreads);

    pool.runGenericParallel(totalTris, [&](unsigned startIdx, unsigned endIdx, unsigned threadIdx) {
        // Reserve an estimated capacity to minimize internal vector reallocations
        perThreadOut[threadIdx].reserve((endIdx - startIdx) / 2);

        for (size_t t = startIdx; t < endIdx; ++t) {
            std::vector<int> tri = model.model.tris[t];
            std::vector<int> texTri = model.model.triTexCoords[t];
            std::vector<Vertex> poly = DataToVertex(tri, texTri, model, transform);

            if (clip.state == ClipState::Intersect) {
                for (int idx : clip.intersectedIndexes) {
                    poly = ClipPolygonPlane(poly, cam.planes[idx]);
                    if (poly.empty()) break;
                }
            }

            if (poly.size() >= 3) {
                FPos3 e1{poly[1].pos.x-poly[0].pos.x, poly[1].pos.y-poly[0].pos.y, poly[1].pos.z-poly[0].pos.z};
                FPos3 e2{poly[2].pos.x-poly[0].pos.x, poly[2].pos.y-poly[0].pos.y, poly[2].pos.z-poly[0].pos.z};
                FPos3 normal = cross(e2, e1);
                FPos3 viewDir = poly[0].pos;

                if (dot(normal, viewDir) >= 0.0f) continue; // Back-face culling

                for (size_t i = 1; i + 1 < poly.size(); i++) {
                    Pos2 p0 = projectVertex(screen, cam.port, F3ToVec4(poly[0].pos));
                    Pos2 p1 = projectVertex(screen, cam.port, F3ToVec4(poly[i].pos));
                    Pos2 p2 = projectVertex(screen, cam.port, F3ToVec4(poly[i+1].pos));

                    if (p0.y == p1.y && p1.y == p2.y) continue;
                    if (isLineTriangle(p0, p1, p2)) continue;

                    float iz0 = 1.0f / poly[0].pos.z;
                    float izI = 1.0f / poly[i].pos.z;
                    float izI1 = 1.0f / poly[i+1].pos.z;

                    RasterTriangle job{
                        p0, p1, p2,
                        iz0, izI, izI1,
                        poly[0].uv.u*iz0, poly[i].uv.u*izI, poly[i+1].uv.u*izI1,
                        poly[0].uv.v*iz0, poly[i].uv.v*izI, poly[i+1].uv.v*izI1,
                        &model.texture,
                        std::min({(int)p0.y, (int)p1.y, (int)p2.y}),
                        std::max({(int)p0.y, (int)p1.y, (int)p2.y})
                    };

                    perThreadOut[threadIdx].push_back(job);
                }
            }
        }
    });

    // Flatten all thread vectors into the main output vector
    size_t totalJobsGenerated = 0;
    for (const auto& vec : perThreadOut) totalJobsGenerated += vec.size();
    out.reserve(totalJobsGenerated);

    for (const auto& vec : perThreadOut) {
        out.insert(out.end(), vec.begin(), vec.end());
    }
}

void rasterizeBand(Screen& screen, const std::vector<RasterTriangle>& jobs,
                    int yStart, int yEnd){
    for (const auto& job : jobs){
        if (job.maxY < yStart || job.minY > yEnd) continue;

		auto texShader = [&job](const ShaderFragment& f, uint32_t colorIn) {
			float u = f.attribs[0] / f.z;
			float v = f.attribs[1] / f.z;
			return job.tex->sample(u, v);
		};

        drawTriangle(screen, job, yStart, yEnd, texShader);
    }
}

void rasterizeParallel(Screen& screen, const std::vector<RasterTriangle>& jobs, RasterPool& pool){
    pool.runFrame(screen.height, [&screen, &jobs](int yStart, int yEnd){
        rasterizeBand(screen, jobs, yStart, yEnd);
    });
}

void renderScene(Scene& scene, Camera& cam, RasterPool& pool){
    scene.screen.clear(scene.bgColor);
    std::vector<RasterTriangle> out;

    for (auto& model : scene.models){
        Mat4x4 translation = makeTranslation(model.worldPos);
        Mat4x4 combined = multiply(translation, model.transform);

        // Call the newly optimized multithreaded projection step
        projectModel(scene.screen, out, cam, model, combined, pool);
    }
    rasterizeParallel(scene.screen, out, pool);
}
