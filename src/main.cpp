#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include "types.hpp"
#include "rendering.hpp"
#include "helper.hpp"

int main(int argc, char* argv[]) {
	Screen screen;
	Renderer renderer(screen.width, screen.height);

	Pos2 p0 = {50, 50};
	Pos2 p1 = {400, 200};
	Pos2 p2 = {70, 250};

    bool running = true;
    while (running) {
		drawFilledTriangle(screen, p0, p1, p2, WHITE);

		// render and do some key press checks
		if (renderer.update(screen)==false) running=false;
		//clear the screen
		std::fill(screen.pixelBuffer.begin(), screen.pixelBuffer.end(), BLACK);
	}
	std::cout << "Averege FPS: " <<  static_cast<int>(renderer.avgFps) << std::endl;

    return 0;
}
