#include <iostream>
#include <vector>
#include <random>

#include "types.hpp"
#include "rendering.hpp"
#include "helper.hpp"
#include "loader.hpp"

int main(int argc, char* argv[]) {
	std::random_device rd;
    std::mt19937 rng(rd());

	Screen screen;
	Viewport port;
	Renderer renderer(screen.width, screen.height);

	ObjModel cubeOBJ = parseObjHeader("src/models/testcube.obj");
	Model cube1 {cubeOBJ, FPos3{-1.5f, -0.5f, 5.0f}};

    bool running = true;
	uint32_t bgColor = Color(45, 45, 45, 255);
	while (running) {

		renderModel(screen, port, cube1);

		// render and do some key press checks
		if (renderer.update(screen)==false) running=false;
		//std::cout << renderer.fps << std::endl;

		//clear the screen
		screen.clear(bgColor);
	}
	std::cout << "Averege FPS: " <<  static_cast<int>(renderer.avgFps) << std::endl;

    return 0;
}
