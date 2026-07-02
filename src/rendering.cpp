#include "rendering.hpp"
#include "iostream"

Renderer::Renderer(int width, int height){
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return;
	}

	if (!SDL_CreateWindowAndRenderer("Software Rasterizer", width, height,
				0, &window, &renderer)) {
		std::cerr << "Window/Renderer Creation Error: " << SDL_GetError() << std::endl;
		return;
	}
	SDL_SetRenderVSync(renderer, 0);

	texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,width,height);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

	if (!texture) {
		std::cerr << "Texture Creation Error: " << SDL_GetError() << std::endl;
		return;
	}

	lastTicks = SDL_GetTicksNS();
}

Renderer::~Renderer(){
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool Renderer::update(Screen& screen){
	Uint64 now = SDL_GetTicksNS();
    Uint64 frameTicks = now - lastTicks;
    lastTicks = now;

    if (frameTicks > 0) {
        deltaTime = frameTicks / 1'000'000'000.0f;
        fps       = 1'000'000'000.0f / frameTicks;
    }

	fpsSamples[fpsHead] = fps;
    fpsHead = (fpsHead + 1) % AVG_FPS_WINDOW;

    float sum = 0.0f;
    for (float s : fpsSamples) sum += s;
    avgFps = sum / AVG_FPS_WINDOW;

	bool running = true;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT) {
			running = false;
		}
		if (event.type == SDL_EVENT_KEY_DOWN){
			if (event.key.key == SDLK_ESCAPE) running=false;
		}
	}

	void* texturePixels = nullptr;
	int pitch = 0;

	if (SDL_LockTexture(texture, nullptr, &texturePixels, &pitch)) {

		if (pitch == screen.width * sizeof(uint32_t)) {
			// If no GPU magic with nice numbers
			std::memcpy(texturePixels, screen.pixelBuffer.data(),
					screen.pixelBuffer.size() * sizeof(uint32_t));
		} else {
			//If my GPU just loves nice numbers too much
			uint8_t* dst = static_cast<uint8_t*>(texturePixels);
			uint8_t* src = reinterpret_cast<uint8_t*>(screen.pixelBuffer.data());
			for (int y = 0; y < screen.height; ++y) {
				std::memcpy(dst, src, screen.width * sizeof(uint32_t));
				dst += pitch;
				src += screen.width * sizeof(uint32_t);
			}
		}

		SDL_UnlockTexture(texture);
	}

	SDL_RenderTexture(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
	return running;
}
