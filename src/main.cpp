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
	Viewport port;
	Renderer renderer(screen.width, screen.height);

	std::uniform_int_distribution<int> randomWidth(10, screen.width-10);
	std::uniform_int_distribution<int> randomHeight(10, screen.height-10);

	FPos3 vAf = {-2.0f, -0.5, 5.0f};
	FPos3 vBf = {-2.0f,  0.5, 5.0f};
	FPos3 vCf = {-1.0f,  0.5, 5.0f};
	FPos3 vDf = {-1.0f, -0.5, 5.0f};

	FPos3 vAb = {-2.0f, -0.5, 6.0f};
	FPos3 vBb = {-2.0f,  0.5, 6.0f};
	FPos3 vCb = {-1.0f,  0.5, 6.0f};
	FPos3 vDb = {-1.0f, -0.5, 6.0f};

    bool running = true;
	uint32_t bgColor = Color(45, 45, 45, 255);
	while (running) {

		//Render the front face
		drawLine(screen, projectVertex(screen, port, vAf), projectVertex(screen, port, vBf), BLUE);
		drawLine(screen, projectVertex(screen, port, vBf), projectVertex(screen, port, vCf), BLUE);
		drawLine(screen, projectVertex(screen, port, vCf), projectVertex(screen, port, vDf), BLUE);
		drawLine(screen, projectVertex(screen, port, vDf), projectVertex(screen, port, vAf), BLUE);

		//Render the back face
		drawLine(screen, projectVertex(screen, port, vAb), projectVertex(screen, port, vBb), RED);
		drawLine(screen, projectVertex(screen, port, vBb), projectVertex(screen, port, vCb), RED);
		drawLine(screen, projectVertex(screen, port, vCb), projectVertex(screen, port, vDb), RED);
		drawLine(screen, projectVertex(screen, port, vDb), projectVertex(screen, port, vAb), RED);

		// Render the sides/connectors between the front and the back
		drawLine(screen, projectVertex(screen, port, vAf), projectVertex(screen, port, vAb), GREEN);
		drawLine(screen, projectVertex(screen, port, vBf), projectVertex(screen, port, vBb), GREEN);
		drawLine(screen, projectVertex(screen, port, vCf), projectVertex(screen, port, vCb), GREEN);
		drawLine(screen, projectVertex(screen, port, vDf), projectVertex(screen, port, vDb), GREEN);

		// render and do some key press checks
		if (renderer.update(screen)==false) running=false;
		//std::cout << renderer.fps << std::endl;

		//clear the screen
		screen.clear(bgColor);
	}
	std::cout << "Averege FPS: " <<  static_cast<int>(renderer.avgFps) << std::endl;

    return 0;
}
