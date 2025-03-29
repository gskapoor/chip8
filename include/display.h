#ifndef DISPLAY_H
#define DISPLAY_H

#include "common.h"

#include <SDL.h>

class Display {
public:
    Display();
    ~Display();

private:
    std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> _window;
    std::unique_ptr<SDL_Surface> _surface;
};


#endif 