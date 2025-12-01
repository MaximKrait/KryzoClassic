#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "utils.h"
#include "enemy.h"

void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& message, int x, int y, SDL_Color color);
void showMenu(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight);
void drawScene(SDL_Renderer* renderer, TTF_Font* font, const GameState& gs, const std::vector<Enemy>& enemies);
bool showGameOver(SDL_Renderer* renderer, TTF_Font* font, int finalScore, int screenWidth, int screenHeight);

