#pragma once
#include "types.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

class Renderer{
	public:
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Texture* texture = nullptr;
		Renderer(int width, int height);
		~Renderer();
		bool update(Screen& screen);
};
