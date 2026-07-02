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
	Camera cam (90, 800.0f/600.0f);
	Renderer renderer(scene.screen.width, scene.screen.height);

	//ObjModel modelOBJ = parseObjHeader("src/models/testcube.obj");
	ObjModel modelOBJ = parseObjHeader("src/models/complextest.obj");

	//Model model {modelOBJ, FPos3{-1.5f, -0.5f, 5.0f}};
	Model model {modelOBJ, FPos3{0.0f, -0.5f, 1.5f}};
	float angle=0;
	model.transform = multiply(model.transform, makeRotationY(angle));

	scene.models.push_back(model);

    bool running = true;
	while (running) {
		//scene.models[0].worldPos.x -= 1*renderer.deltaTime;
		angle+=30*renderer.deltaTime;

		scene.models[0].transform = multiply(Identity4x4, makeRotationY(angle));

		renderScene(scene, cam);

		// render and do some key press checks
		if (renderer.update(scene.screen)==false) running=false;
		//std::cout << renderer.fps << std::endl;

		//clear the screen
	}
	std::cout << "Averege FPS: " <<  static_cast<int>(renderer.avgFps) << std::endl;

    return 0;
}
