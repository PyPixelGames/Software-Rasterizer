#include <iostream>
#include <vector>
#include <random>

#include "types.hpp"
#include "rendering.hpp"
#include "helper.hpp"

int main(int argc, char* argv[]) {
	std::random_device rd;
    std::mt19937 rng(rd());

	Screen screen;
	Renderer renderer(screen.width, screen.height);

	std::uniform_int_distribution<int> randomWidth(10, screen.width-10);
	std::uniform_int_distribution<int> randomHeight(10, screen.height-10);

	Pos2 p0{100, 100};
	Pos2 p1{600, 400};
	Pos2 p2{100, 400};

    bool running = true;
	uint32_t bgColor = Color(45, 45, 45, 255);
    while (running) {
		drawShadedTriangle(screen, p0, 0.0f, p1, 1.0f, p2, 0.5f, GREEN);

		// render and do some key press checks
		if (renderer.update(screen)==false) running=false;
		//std::cout << renderer.fps << std::endl;

		//clear the screen
		screen.clear(bgColor);
	}
	std::cout << "Averege FPS: " <<  static_cast<int>(renderer.avgFps) << std::endl;

    return 0;
}
