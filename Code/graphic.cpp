#include "graphic.h"
#include "anim.h"
#include <iostream>
#include <SDL2/SDL2_gfxPrimitives.h>

void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& message, int x, int y, SDL_Color color) {
    if (!font) { std::cerr << "Font not loaded\n"; return; }
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_Rect rect = { x, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

void renderTextCenter(SDL_Renderer* renderer, TTF_Font* font,
                      const std::string& text, int y, SDL_Color color,
                      int screenWidth)
{
    int w, h;
    TTF_SizeText(font, text.c_str(), &w, &h);

    int x = (screenWidth - w) / 2;

    SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_Rect dst = { x, y, w, h };
    SDL_RenderCopy(renderer, tex, nullptr, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void showMenu(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight) {
    bool open = true;
    SDL_Event ev;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gray = {180, 180, 180, 255};

    TTF_Font* smallFont = TTF_OpenFont("Assets/Fonts/VCR_OSD_MONO_1.001.ttf", 16);
    if (!smallFont) std::cerr << "Small font load failed: " << TTF_GetError() << "\n";

    SDL_Surface* logoSurface = SDL_LoadBMP("Assets/UI/logo.bmp");
    SDL_Texture* logoTex = nullptr;

    if (logoSurface) {
        Uint32 colorkey = SDL_MapRGB(logoSurface->format, 255, 0, 255);
        SDL_SetColorKey(logoSurface, SDL_TRUE, colorkey);

        logoTex = SDL_CreateTextureFromSurface(renderer, logoSurface);
        SDL_FreeSurface(logoSurface);
    } else {
        std::cerr << "Logo load failed: " << SDL_GetError() << "\n";
    }

    SDL_Rect logoRect;
    logoRect.w = 256;
    logoRect.h = 92;
    logoRect.x = screenWidth/2 - logoRect.w/2;
    logoRect.y = screenHeight/2 - 100;

    while (open) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) exit(0);
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_RETURN)
                open = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (logoTex)
            SDL_RenderCopy(renderer, logoTex, nullptr, &logoRect);

        renderTextCenter(renderer, font, "Movement with WASD",
                         screenHeight/2 + 80, white, screenWidth);

        renderTextCenter(renderer, font, "Press ENTER to start the game",
                         screenHeight/2 + 120, white, screenWidth);

        if (smallFont){
            renderTextCenter(renderer, smallFont, "Game by Maxim Krait", screenHeight - 50, gray, screenWidth);
            renderTextCenter(renderer, smallFont, "v1.1.0", screenHeight - 30, gray, screenWidth);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (logoTex)
        SDL_DestroyTexture(logoTex);
    if (smallFont)
        TTF_CloseFont(smallFont);
}

void drawScene(SDL_Renderer* renderer, TTF_Font* font, const GameState& gs, const std::vector<Enemy>& enemies) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color playerColor = {0, 255, 0, 255};

    if (gs.hit) {
        SDL_Color tmp;
        if (getHitDrawColor(gs.hitStart, SDL_GetTicks(), tmp))
            playerColor = tmp;
    }

    filledCircleRGBA(
        renderer,
        gs.playerX + PLAYER_SIZE / 2,
        gs.playerY + PLAYER_SIZE / 2,
        PLAYER_SIZE / 2,
        playerColor.r, playerColor.g, playerColor.b, playerColor.a
    );

    renderEnemies(enemies, renderer, gs);

    renderText(renderer, font, "Score: " + std::to_string(gs.score), 10, 10, {255,255,255,255});

    renderText(renderer, font, "HP: ", 10, 40, {255,255,255,255});

    int w, h;
    TTF_SizeText(font, "HP: ", &w, &h);

    SDL_Color hpColor;
    getHPDrawColor(gs.hp, gs.hpCriticalStart, SDL_GetTicks(), hpColor);
    renderText(renderer, font, std::to_string(gs.hp), 10 + w, 40, hpColor);

    if (gs.heal.active) {
        SDL_Rect r{gs.heal.x, gs.heal.y, gs.heal.size, gs.heal.size};
        SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
        SDL_RenderFillRect(renderer, &r);
    }

    if (gs.criticalActive) {
        Uint32 t = SDL_GetTicks() - gs.criticalStart;
        if (t < 600) {
            SDL_Color critColor = {255, 70, 0, 255};
            renderText(renderer, font, "CRITICAL!", gs.criticalX, gs.criticalY, critColor);
        }
    }

    SDL_RenderPresent(renderer);
}

bool showGameOver(SDL_Renderer* renderer, TTF_Font* font,
                  int finalScore, int screenWidth, int screenHeight)
{
    SDL_Event ev;
    SDL_Color red   = {255, 0, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gray  = {200, 200, 200, 255};

    while (true) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) return false;
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_RETURN)
                return true;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderTextCenter(renderer, font, "Game Over!",
                         screenHeight/2 - 60, red, screenWidth);

        renderTextCenter(renderer, font,
                         "Final Score: " + std::to_string(finalScore),
                         screenHeight/2 - 20, white, screenWidth);

        renderTextCenter(renderer, font, "Press ENTER to restart",
                         screenHeight/2 + 40, gray, screenWidth);

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
}

