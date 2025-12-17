#include "display.h"
#include "utils.h"
#include <iostream>

bool initDisplay(Display& d) {
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    d.window = SDL_CreateWindow(
        "Kryzo Classic",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        dm.w,
        dm.h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    if (!d.window) return false;

    d.renderer = SDL_CreateRenderer(
        d.window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!d.renderer) return false;

    SDL_RenderSetLogicalSize(
        d.renderer,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    SDL_RenderSetIntegerScale(d.renderer, SDL_FALSE);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    return true;
}

void shutdownDisplay(Display& d) {
    if (d.renderer) SDL_DestroyRenderer(d.renderer);
    if (d.window) SDL_DestroyWindow(d.window);
}

