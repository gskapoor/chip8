#ifndef DISPLAY_H
#define DISPLAY_H

#include "common.h"

#include <SDL.h>

class Display {
public:
    Display();
    ~Display();
    void update(std::array<std::array<uint8_t, 64 / 8 >, 32>);
private:
    std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> _window;
    std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer*)> _renderer;
    std::unique_ptr<SDL_Texture, void (*)(SDL_Texture*)> _texture;

    std::array<uint32_t, 64 * 32> _textureBuffer;
};


#endif 