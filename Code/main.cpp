#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include "display.h"
#include "utils.h"
#include "pascal.h"
#include "enemy.h"
#include "graphic.h"
#include "anim.h"

int main(int argc, char* argv[]) {
    initRandom();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << SDL_GetError() << "\n";
        return -1;
    }

    if (TTF_Init() == -1) {
        std::cerr << TTF_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    Display disp;
    if (!initDisplay(disp)) {
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("Assets/Fonts/VCR_OSD_MONO_1.001.ttf", 28);
    if (!font) {
        std::cerr << TTF_GetError() << "\n";
        shutdownDisplay(disp);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    GameState gs;
    gs.screenW = WINDOW_WIDTH;
    gs.screenH = WINDOW_HEIGHT;
    gs.lastScoreUpdate = SDL_GetTicks();

    std::vector<Enemy> enemies;
    initEnemies(enemies, NUM_ENEMIES, gs.screenW, gs.screenH);

    showMenu(disp.renderer, font, gs.screenW, gs.screenH);

    bool quit = false;
    SDL_Event ev;

    while (!quit) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT)
                quit = true;
        }

        movePlayer(gs);
        trySpawnHeal(gs);
        checkHealPickup(gs);

        if (handleCollisions(enemies, gs)) {
            bool restart = showGameOver(
                disp.renderer,
                font,
                gs.score,
                gs.screenW,
                gs.screenH
            );

            if (!restart)
                break;

            gs = GameState();
            gs.screenW = WINDOW_WIDTH;
            gs.screenH = WINDOW_HEIGHT;
            gs.hp = gs.maxHP;
            gs.lastScoreUpdate = SDL_GetTicks();

            initEnemies(enemies, NUM_ENEMIES, gs.screenW, gs.screenH);
        }

        updateEnemies(enemies, gs);

        if (SDL_GetTicks() - gs.lastScoreUpdate >= SCORE_INTERVAL) {
            gs.score++;
            gs.lastScoreUpdate = SDL_GetTicks();
        }

        drawScene(disp.renderer, font, gs, enemies);

        SDL_Delay(FRAME_DELAY);
    }

    TTF_CloseFont(font);
    shutdownDisplay(disp);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
