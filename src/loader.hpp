#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "types.hpp"

// Function to parse the OBJ file up until materials/faces
ObjModel parseObjHeader(const std::string& filename);
