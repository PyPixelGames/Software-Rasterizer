#include "loader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


ObjModel parseObjHeader(const std::string& filename) {
    std::ifstream file(filename);
    ObjModel data;
	std::vector<int> faces;

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        // Ignore comments
        if (prefix == "#") {
            continue;
        }
        // Object name
        else if (prefix == "o") {
            ss >> data.objName;
        }
        // Geometric Vertices
        else if (prefix == "v") {
            FPos3 v;
            ss >> v.x >> v.y >> v.z;
			v.z = -v.z;
            data.vertices.push_back(v);
        }
        // Texture Coordinates
        else if (prefix == "vt") {
            TextureCoord vt;
            ss >> vt.u >> vt.v;
            data.texCoords.push_back(vt);
        }
        // Vertex Normals
        else if (prefix == "vn") {
            FPos3 vn;
            ss >> vn.x >> vn.y >> vn.z;
			vn.z = -vn.z;
            data.normals.push_back(vn);
        }
		// Faces
		// Faces
		else if (prefix == "f"){
			std::string vertexStr;
			std::vector<int> vIdx, vtIdx;
			while (ss >> vertexStr) {
				int v=0, vt=0, vn=0;
				// format can be v, v/vt, v/vt/vn, or v//vn
				size_t p1 = vertexStr.find('/');
				v = std::stoi(vertexStr.substr(0, p1));
				if (p1 != std::string::npos) {
					size_t p2 = vertexStr.find('/', p1+1);
					std::string vtStr = vertexStr.substr(p1+1, p2-p1-1);
					if (!vtStr.empty()) vt = std::stoi(vtStr);
				}
				vIdx.push_back(v);
				vtIdx.push_back(vt);
			}
			// triangulate both vIdx and vtIdx in parallel (fan)
			for (size_t i=1; i+1<vIdx.size(); i++){
				data.tris.push_back({vIdx[0], vIdx[i], vIdx[i+1]});
				data.triTexCoords.push_back({vtIdx[0], vtIdx[i], vtIdx[i+1]});
			}
		}
	}

	FPos3 avg;
	for (auto v: data.vertices){
		avg.x += v.x;
		avg.y += v.y;
		avg.z += v.z;
	}
	avg.x = avg.x/size(data.vertices);
	avg.y = avg.y/size(data.vertices);
	avg.z = avg.z/size(data.vertices);

	float r = 0;
	for (auto v: data.vertices){
		float dis = std::sqrt(
				(std::pow((v.x-avg.x), 2))+
				(std::pow((v.y-avg.y), 2))+
				(std::pow((v.z-avg.z), 2)));
		r = std::max(r, dis);
	}

	data.sphere_radius = r;
	data.sphere_center = avg;

    file.close();
    return data;
}

Texture loadTexture(const std::string& path){
    int w, h, ch;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!data){
        std::cerr << "Failed to load texture: " << path << " (" << stbi_failure_reason() << ")" << std::endl;
        return Texture{0, 0, {}};
    }
    Texture tex{w, h, std::vector<uint32_t>(w*h)};
    for (int i = 0; i < w*h; i++){
        uint8_t r=data[i*4+0], g=data[i*4+1], b=data[i*4+2], a=data[i*4+3];
        tex.pixels[i] = Color(r,g,b,a);
    }
    stbi_image_free(data);
    return tex;
}
