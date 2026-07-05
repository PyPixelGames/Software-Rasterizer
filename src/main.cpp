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
	Camera cam (90, static_cast<float>(scene.screen.width)/scene.screen.height);
	//cam.planes[0].D = -1;
	Renderer renderer(scene.screen.width, scene.screen.height);

	ObjModel modelOBJ = parseObjHeader("src/models/testcube.obj");
	//ObjModel modelOBJ = parseObjHeader("src/models/couch.obj");

	//Model model {modelOBJ, FPos3{-1.5f, -0.5f, 5.0f}};
	Model model {modelOBJ, FPos3{0.0f, -1.5f, 3.0f}};
	model.texture = loadTexture("src/models/testcubetexture.png");
	float angle=0;
	model.transform = multiply(model.transform, makeRotationY(angle));

	scene.models.push_back(model);

    bool running = true;
	while (running) {
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
