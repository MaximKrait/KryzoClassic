#include "display.h"
#include "utils.h"
#include "log.h"
#include <SDL2/SDL.h>

bool initDisplay(Display& d) {
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) != 0) {
        LOG("SDL_GetCurrentDisplayMode failed: " + std::string(SDL_GetError()));
        return false;
    }
    LOG("Current display mode: " + std::to_string(dm.w) + "x" + std::to_string(dm.h));

    d.window = SDL_CreateWindow(
        "Kryzo Classic",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        dm.w,
        dm.h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    if (!d.window) {
        LOG("SDL_CreateWindow failed: " + std::string(SDL_GetError()));
        return false;
    }
    LOG("Window created");

    d.renderer = SDL_CreateRenderer(
        d.window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!d.renderer) {
        LOG("SDL_CreateRenderer failed: " + std::string(SDL_GetError()));
        SDL_DestroyWindow(d.window);
        return false;
    }
    LOG("Renderer created");

    if (SDL_RenderSetLogicalSize(d.renderer, WINDOW_WIDTH, WINDOW_HEIGHT) != 0) {
        LOG("SDL_RenderSetLogicalSize failed: " + std::string(SDL_GetError()));
    } else {
        LOG("Logical size set to " + std::to_string(WINDOW_WIDTH) + "x" + std::to_string(WINDOW_HEIGHT));
    }

    if (SDL_RenderSetIntegerScale(d.renderer, SDL_FALSE) != 0) {
        LOG("SDL_RenderSetIntegerScale failed: " + std::string(SDL_GetError()));
    } else {
        LOG("Integer scale set");
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
        LOG("SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY) failed");
    } else {
        LOG("Render scale quality hint set to 0");
    }

    return true;
}

void shutdownDisplay(Display& d) {
    if (d.renderer) {
        SDL_DestroyRenderer(d.renderer);
        LOG("Renderer destroyed");
    }
    if (d.window) {
        SDL_DestroyWindow(d.window);
        LOG("Window destroyed");
    }
}