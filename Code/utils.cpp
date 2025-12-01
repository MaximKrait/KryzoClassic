#include "utils.h"
#include <SDL2/SDL.h>

void drawBonus(SDL_Renderer* renderer, const Bonus& b) {
    if (!b.active) return;
    SDL_Rect r{ b.x, b.y, b.size, b.size };
    SDL_SetRenderDrawColor(renderer, 0, 200, 100, 255);
    SDL_RenderFillRect(renderer, &r);
}

void trySpawnHeal(GameState& gs) {
    Bonus& heal = gs.heal;
    if (heal.active) return;
    if (gs.hp >= 60) return;

    if (SDL_GetTicks() - gs.lastScoreUpdate < 500) return;

    if (randomInt(HEAL_SPAWN_CHANCE) == 50) {
        heal.active = true;
        heal.x = randomInt(std::max(1, gs.screenW - heal.size));
        heal.y = randomInt(std::max(1, gs.screenH - heal.size));

        SDL_Rect p{ gs.playerX, gs.playerY, PLAYER_SIZE, PLAYER_SIZE };
        SDL_Rect b{ heal.x, heal.y, heal.size, heal.size };
        if (SDL_HasIntersection(&p, &b)) {
            heal.x = std::min(gs.screenW - heal.size, heal.x + PLAYER_SIZE);
            heal.y = std::min(gs.screenH - heal.size, heal.y + PLAYER_SIZE);
        }

        std::cout << "[DEBUG] Heal spawned at: " << heal.x << "," << heal.y << " (hp=" << gs.hp << ")\n";
    }
}

void checkHealPickup(GameState& gs) {
    Bonus& heal = gs.heal;
    if (!heal.active) return;

    SDL_Rect p{ gs.playerX, gs.playerY, PLAYER_SIZE, PLAYER_SIZE };
    SDL_Rect b{ heal.x, heal.y, heal.size, heal.size };

    if (SDL_HasIntersection(&p, &b)) {
        gs.hp += 20;
        if (gs.hp > gs.maxHP) gs.hp = gs.maxHP;
        heal.active = false;
        std::cout << "[DEBUG] Heal picked. hp=" << gs.hp << "\n";
    }
}

