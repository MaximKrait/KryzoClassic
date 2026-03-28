#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "display.h"
#include "utils.h"
#include "pascal.h"
#include "enemy.h"
#include "graphic.h"
#include "anim.h"

//Debug log
std::ofstream logFile("debug.log");
#define LOG(x) do { std::cerr << x << std::endl; logFile << x << std::endl; } while(0)

int main(int argc, char* argv[]) {
    LOG("о7");

    initRandom();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG("SDL_Init FAILED: " << SDL_GetError());
        return -1;
    }
    LOG("SDL initialized");

    if (TTF_Init() == -1) {
        LOG("TTF_Init FAILED: " << TTF_GetError());
        SDL_Quit();
        return -1;
    }
    LOG("TTF initialized");

    Display disp;
    if (!initDisplay(disp)) {
        LOG("initDisplay FAILED");
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    LOG("Display initialized");

    TTF_Font* font = TTF_OpenFont("Assets/Fonts/VCR_OSD_MONO_1.001.ttf", 28);
    if (!font) {
        LOG("Font load FAILED: " << TTF_GetError());
        shutdownDisplay(disp);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    LOG("Font loaded");

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
            LOG("Player died. Score: " << gs.score);

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

        const char* err = SDL_GetError();
        if (err && err[0] != '\0') {
            LOG("SDL runtime error: " << err);
            SDL_ClearError();
        }
    }

    LOG("Shutting down...");

    TTF_CloseFont(font);
    shutdownDisplay(disp);
    TTF_Quit();
    SDL_Quit();

    LOG("o7");

    return 0;
}