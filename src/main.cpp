#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <vector>
#include <iostream>
#include <cstring>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!SDL_CreateWindowAndRenderer("Software Rasterizer", SCREEN_WIDTH, SCREEN_HEIGHT, 0,
				&window, &renderer)) {
        std::cerr << "Window/Renderer Creation Error: " << SDL_GetError() << std::endl;
        return 1;
    }
	SDL_SetRenderVSync(renderer, 1);

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    if (!texture) {
        std::cerr << "Texture Creation Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::vector<uint32_t> pixelBuffer(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFF000000); // Start with black

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
			if (event.type == SDL_EVENT_KEY_DOWN){
				if (event.key.key == SDLK_ESCAPE) running=false;
			}
        }

        static uint8_t colorOffset = 0;
        colorOffset++;
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
            pixelBuffer[i] = (0xFF << 24) | (colorOffset << 16) | (225 << 8) | colorOffset;
        }

        void* texturePixels = nullptr;
        int pitch = 0;

        if (SDL_LockTexture(texture, nullptr, &texturePixels, &pitch)) {

            if (pitch == SCREEN_WIDTH * sizeof(uint32_t)) {
				// If no GPU magic with nice numbers
                std::memcpy(texturePixels, pixelBuffer.data(), pixelBuffer.size() * sizeof(uint32_t));
            } else {
				//If my GPU just loves nice numbers too much
                uint8_t* dst = static_cast<uint8_t*>(texturePixels);
                uint8_t* src = reinterpret_cast<uint8_t*>(pixelBuffer.data());
                for (int y = 0; y < SCREEN_HEIGHT; ++y) {
                    std::memcpy(dst, src, SCREEN_WIDTH * sizeof(uint32_t));
                    dst += pitch;
                    src += SCREEN_WIDTH * sizeof(uint32_t);
                }
            }

            SDL_UnlockTexture(texture);
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
