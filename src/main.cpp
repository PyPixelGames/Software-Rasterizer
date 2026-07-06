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
	Renderer renderer(scene.screen.width, scene.screen.height);

	RasterPool pool(scene.screen.height);

	ObjModel modelOBJ = parseObjHeader("src/models/complex.obj");
	Model model {modelOBJ, FPos3{0.0f, -0.5f, 5.0f}};

	model.texture = loadTexture("src/models/complextexture.png");

	float angle=0;
	model.transform = multiply(model.transform, makeRotationY(angle));
	scene.models.push_back(model);

	scene.lights.push_back(Light{
			LightType::Directional,
			FPos3{-0.4f, 0.8f, -0.4f}, //Direction
			{},						   // Position (for the point light
			0.9f,					   // Intensity
			0.0f,					   // range 0.0 meaning infinite
			WHITE					   // Color
			});

    bool running = true;
	while (running) {
		angle+=30*renderer.deltaTime;

		scene.models[0].transform = multiply(Identity4x4, makeRotationY(angle));

		renderScene(scene, cam, pool);

		// render and do some key press checks
		if (renderer.update(scene.screen)==false) running=false;
		//std::cout << renderer.fps << std::endl;

		//clear the screen
	}
	std::cout << "Averege FPS: " <<  static_cast<int>(renderer.avgFps) << std::endl;

    return 0;
}
