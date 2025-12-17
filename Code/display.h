#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct Display {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

bool initDisplay(Display& d);
void shutdownDisplay(Display& d);

