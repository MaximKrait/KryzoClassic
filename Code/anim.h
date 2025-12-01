#pragma once
#include <SDL2/SDL.h>

bool getHitDrawColor(Uint32 hitStart, Uint32 currentTime, SDL_Color& outColor);
bool getHPDrawColor(int hp, Uint32 hpCriticalStart, Uint32 currentTime, SDL_Color& outColor);

