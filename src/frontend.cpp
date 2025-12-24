#include "frontend.h"
#include <iostream>

SDLFrontend::SDLFrontend()
    : window(nullptr)
    , renderer(nullptr)
{
}

SDLFrontend::~SDLFrontend() {
    cleanup();
}

bool SDLFrontend::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool SDLFrontend::createWindow(const std::string& title, int width, int height, bool resizable) {
    const Uint32 flags = resizable ? SDL_WINDOW_RESIZABLE : 0;
    window = SDL_CreateWindow(title.c_str(), width, height, flags);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool SDLFrontend::pollEvent(SDL_Event& event) {
    return SDL_PollEvent(&event);
}

void SDLFrontend::delay(Uint32 ms) {
    SDL_Delay(ms);
}

void SDLFrontend::clear(int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
}

void SDLFrontend::present() {
    SDL_RenderPresent(renderer);
}

SDL_Renderer* SDLFrontend::getRenderer() {
    return renderer;
}

void SDLFrontend::cleanup() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}
