#pragma once
#include "types.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

class Renderer{
	public:
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Texture* texture = nullptr;

		Uint64 lastTicks;

		float fps;
		float deltaTime;
		float avgFps=0;
		float avgFpsCounter;
		int avgFpsCycles=50;
		int counter=0;

		Renderer(int width, int height);
		~Renderer();
		bool update(Screen& screen);
};
