#include "display.h"

Display::Display()
    : _window(
        nullptr,
        SDL_DestroyWindow
    ),
    _surface(
        nullptr
    ) 
{

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        throw std::runtime_error("Error Initializing SDL");
    }

    _window.reset(
        SDL_CreateWindow( 
            "chip8", 
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED, 
            1280, 720, 
            SDL_WINDOW_SHOWN )
    );

    _surface.reset(
        SDL_GetWindowSurface(_window.get())
    );

    SDL_FillRect( _surface.get(), NULL, SDL_MapRGB( _surface->format, 255, 0, 0) );

	// // Update the window display
	SDL_UpdateWindowSurface( _window.get() );

    system("sleep 5");


}

Display::~Display(){

    SDL_Quit();

}