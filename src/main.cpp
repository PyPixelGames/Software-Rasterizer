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

	Scene scene;
	Camera cam;
	Renderer renderer(scene.screen.width, scene.screen.height);

	ObjModel cubeOBJ = parseObjHeader("src/models/testcube.obj");

	Model cube1 {cubeOBJ, FPos3{-1.5f, -0.5f, 5.0f}};
	float angle=0;
	cube1.transform = multiply(cube1.transform, makeRotationY(angle));

	scene.models.push_back(cube1);

    bool running = true;
	while (running) {
		scene.models[0].transform = multiply(Identity4x4,
				makeRotationY(angle));
		angle+=0.5;
		renderScene(scene, cam);

		// render and do some key press checks
		if (renderer.update(scene.screen)==false) running=false;
		//std::cout << renderer.fps << std::endl;

		//clear the screen
	}
	std::cout << "Averege FPS: " <<  static_cast<int>(renderer.avgFps) << std::endl;

    return 0;
}
