#include "anim.h"
#include "utils.h"

static const Uint32 HIT_BLINK_DURATION = 500;

bool getHitDrawColor(Uint32 hitStart, Uint32 currentTime, SDL_Color& outColor) {
    Uint32 t = currentTime - hitStart;

    if (t < HIT_BLINK_DURATION) {
        if (((t / 160) % 2) == 0)
            outColor = {255, 255, 255, 255};
        else
            outColor = {0, 255, 0, 255};
        return true;
    }

    return false;
}

static const Uint32 HP_BLINK_DURATION = 300;

bool getHPDrawColor(int hp, Uint32 hpCriticalStart, Uint32 currentTime, SDL_Color& outColor) {
    if (hp > 25) {
        outColor = {255, 255, 255, 255};
        return false;
    } 
    else if (hp > 10) {
        outColor = {255, 0, 0, 255};
        return false;
    } 
    else { // hp <= 10
        Uint32 t = currentTime - hpCriticalStart;
        if (((t / HP_BLINK_DURATION) % 2) == 0)
            outColor = {255, 0, 0, 255};
        else
            outColor = {0, 0, 0, 255};
        return true;
    }
}

