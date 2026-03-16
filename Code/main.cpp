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
    gs.playerPosX = static_cast<float>(gs.playerX);
    gs.playerPosY = static_cast<float>(gs.playerY);

    std::vector<Enemy> enemies;
    initEnemies(enemies, NUM_ENEMIES, gs.screenW, gs.screenH);

    showMenu(disp.renderer, font, gs.screenW, gs.screenH);

    bool quit = false;
    SDL_Event ev;
    
    Uint32 previousFrameTime = SDL_GetTicks();

    while (!quit) {
        Uint32 now = SDL_GetTicks();
        float deltaTime = (now - previousFrameTime) / 16.6667f;
        if (deltaTime < 0.0f) deltaTime = 0.0f;
        if (deltaTime > 3.0f) deltaTime = 3.0f;
        previousFrameTime = now;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT)
                quit = true;
        }

        movePlayer(gs, deltaTime);
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
            gs.playerPosX = static_cast<float>(gs.playerX);
            gs.playerPosY = static_cast<float>(gs.playerY);

            initEnemies(enemies, NUM_ENEMIES, gs.screenW, gs.screenH);
            previousFrameTime = SDL_GetTicks();
        }

        updateEnemies(enemies, gs, deltaTime);

        if (SDL_GetTicks() - gs.lastScoreUpdate >= SCORE_INTERVAL) {
            gs.score++;
            gs.lastScoreUpdate = SDL_GetTicks();
        }

        drawScene(disp.renderer, font, gs, enemies);

    }

    TTF_CloseFont(font);
    shutdownDisplay(disp);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
