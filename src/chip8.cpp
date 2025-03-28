#include "../include/common.h"
#include <SDL.h>

int main(){

    std::cout << "hello world" << std::endl;

    SDL_Surface* winSurface = NULL;
    SDL_Window* window = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Error Initializing SDL" << std::endl;
        return 1;
    }

    window = SDL_CreateWindow( "Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN );
    winSurface = SDL_GetWindowSurface( window );

    SDL_FillRect( winSurface, NULL, SDL_MapRGB( winSurface->format, 255, 0, 0) );

	// Update the window display
	SDL_UpdateWindowSurface( window );

    system("sleep 5");

	// Destroy the window. This will also destroy the surface
	SDL_DestroyWindow( window );

	// Quit SDL
	SDL_Quit();
	
	// End the program
	return 0;



}