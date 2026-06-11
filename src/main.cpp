#include <iostream>
#include <vector>

#include "types.hpp"
#include "rendering.hpp"

int main(int argc, char* argv[]) {
	Screen screen;
	Renderer renderer(screen.width, screen.height);

    bool running = true;
    while (running) {
		static uint8_t colorOffset = 0;
		colorOffset++;
		for (int i = 0; i < screen.width * screen.height; ++i) {
			screen.pixelBuffer[i] = (0xFF << 24) | (colorOffset << 16) | (225 << 8) | colorOffset;
		}

		if (renderer.update(screen)==false) running=false;
	}


    return 0;
}
