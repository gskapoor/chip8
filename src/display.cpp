#include "display.h"

Display::Display()
    : _window(
        nullptr,
        SDL_DestroyWindow
    ),
    _renderer(
        nullptr,
        SDL_DestroyRenderer
    ),
    _texture(
        nullptr,
        SDL_DestroyTexture
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

    assert(_window != nullptr);

    _renderer.reset(
        SDL_CreateRenderer(
            _window.get(),
            -1,
            SDL_RENDERER_ACCELERATED
            // TODO: SEE WHAT ELSE WOULD WORK
        )
    );

    assert(_renderer != nullptr);

    _texture.reset(
        SDL_CreateTexture(
            _renderer.get(),
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            64,
            32
        )
    );
    if (!_texture) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
        throw std::runtime_error("Error creating texture");
    }

    SDL_RenderSetLogicalSize(_renderer.get(), 64, 32);

}

Display::~Display(){

    SDL_Quit();

}

void Display::update(std::array<std::array<uint8_t, 64 / 8>, 32> buffer) {

    std::cout << __FILE__ << " " << __LINE__ << std::endl;
    void* pixels = nullptr;
    int pitch = 0;

    if (SDL_LockTexture(_texture.get(), nullptr, &pixels, &pitch) != 0) {
        std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << "\n";
        return;
    }

    uint32_t* pixel_ptr = static_cast<uint32_t*>(pixels);

    for (size_t y = 0; y < 32; ++y) {

        for (size_t byte = 0; byte < 8; ++byte) {

            uint8_t byte_val = buffer[y][byte];
            for (int bit = 0; bit < 8; ++bit) {
                size_t x = byte * 8 + (size_t)(7 - bit); // CHIP-8 is MSB-first
                bool on = (byte_val >> bit) & 1;

                uint32_t color = on ? 0xFFFFFFFF : 0x000000FF; // white or black (RGBA)
                pixel_ptr[y * ((size_t)pitch / 4) + x] = color;
            }
        }
    }

    SDL_UnlockTexture(_texture.get());

    // Complete the render
    SDL_RenderClear(_renderer.get()); // clear the screen
    SDL_RenderCopy(_renderer.get(), _texture.get(), nullptr, nullptr); // draw the texture
    SDL_RenderPresent(_renderer.get()); // show it


}
