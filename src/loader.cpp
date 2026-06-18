#include "loader.hpp"

ObjModel parseObjHeader(const std::string& filename) {
    std::ifstream file(filename);
    ObjModel data;

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
            Vertex v;
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
            Normal vn;
            ss >> vn.x >> vn.y >> vn.z;
            data.normals.push_back(vn);
        }
        // Stop reading once we reach material assignments or face definitions
        else if (prefix == "usemtl" || prefix == "f") {
            std::cout << "Reached material/face definitions ('" << prefix << "'). Stopping parser.\n";
            break;
        }
    }

    file.close();
    return data;
}
