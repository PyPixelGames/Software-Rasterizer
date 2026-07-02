#include "loader.hpp"

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
            data.normals.push_back(vn);
        }
		// Faces
		else if (prefix == "f"){
			// get the first nu,bers of a sentance like this:
			// f 2/1/1 1/2/1 3/3/1 4/4/1
			// and get all the first numbers as intagers (2, 1, 3, 4)

			std::string vertexStr;
			faces.clear();
            while (ss >> vertexStr) {
                std::stringstream vertexSS(vertexStr);
                int vertexIndex;

                // Read the integer up to the first '/' character
                if (vertexSS >> vertexIndex) {
                   faces.push_back(vertexIndex);
				}
            }
			if (faces.size()==3){
				data.tris.push_back(std::vector<int>{faces[0], faces[1], faces[2]});
			}else{
				data.tris.push_back(std::vector<int>{faces[0], faces[1], faces[2]});
				data.tris.push_back(std::vector<int>{faces[0], faces[2], faces[3]});
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
	std::cout << r << std::endl;

    file.close();
    return data;
}
