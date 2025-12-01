#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include "utils.h"
#include "pascal.h"
#include "enemy.h"
#include "graphic.h"
#include "anim.h"

int main(int argc, char* argv[]) {
    initRandom();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << "\n";
        return -1;
    }
    if (TTF_Init() == -1) {
        std::cerr << "TTF init failed: " << TTF_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Kryzo Classic", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("Assets/Fonts/VCR_OSD_MONO_1.001.ttf", 28);
    if (!font) {
        std::cerr << "Font load failed: " << TTF_GetError() << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    GameState gs;
    gs.screenW = WINDOW_WIDTH;
    gs.screenH = WINDOW_HEIGHT;
    gs.lastScoreUpdate = SDL_GetTicks();
    gs.lastCollision = 0;

    std::vector<Enemy> enemies;
    initEnemies(enemies, NUM_ENEMIES, gs.screenW, gs.screenH);

    showMenu(renderer, font, gs.screenW, gs.screenH);

    bool quit = false;
    SDL_Event ev;
    Uint32 lastTick = SDL_GetTicks();

    while (!quit) {
        Uint32 now = SDL_GetTicks();
        [[maybe_unused]] Uint32 delta = now - lastTick;
        lastTick = now;

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                quit = true;
            }
        }

        movePlayer(gs);
        trySpawnHeal(gs);
        checkHealPickup(gs);

        bool gameOver = handleCollisions(enemies, gs);
        if (gameOver) {
            bool restart = showGameOver(renderer, font, gs.score, gs.screenW, gs.screenH);
            if (restart) {
                gs = GameState();
                gs.screenW = WINDOW_WIDTH;
                gs.screenH = WINDOW_HEIGHT;
                gs.hp = gs.maxHP;
                gs.lastScoreUpdate = SDL_GetTicks();
                gs.lastCollision = 0;
                initEnemies(enemies, NUM_ENEMIES, gs.screenW, gs.screenH);
                continue;
            } else {
                quit = true;
                break;
            }
        }

        updateEnemies(enemies, gs);

        if (now - gs.lastScoreUpdate >= SCORE_INTERVAL) {
            gs.score++;
            gs.lastScoreUpdate = now;
        }

        drawScene(renderer, font, gs, enemies);

        SDL_Delay(FRAME_DELAY);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

