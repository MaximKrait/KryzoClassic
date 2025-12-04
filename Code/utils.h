#pragma once
#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

inline constexpr int WINDOW_WIDTH = 1024;
inline constexpr int WINDOW_HEIGHT = 576;
inline constexpr int PLAYER_SIZE = 20;
inline constexpr int PLAYER_SPEED = 5;
inline constexpr Uint32 FRAME_DELAY = 16;
inline constexpr int ENEMY_SIZE = 20;
inline constexpr int ENEMY_SPEED = 2;
inline constexpr int NUM_ENEMIES = 5;
inline constexpr Uint32 SCORE_INTERVAL = 1000;
inline constexpr Uint32 COLLISION_COOLDOWN = 100;
inline constexpr int HEAL_SPAWN_CHANCE = 500;

struct Bonus {
    int x = -1000;
    int y = -1000;
    int size = 20;
    bool active = false;
};

struct GameState {
    int playerX = WINDOW_WIDTH / 2;
    int playerY = WINDOW_HEIGHT / 2;
    int score = 0;
    int hp = 100;
    int maxHP = 100;
    Uint32 lastScoreUpdate = 0;
    Uint32 lastCollision = 0;
    Uint32 hpCriticalStart = 0;
    int screenW = WINDOW_WIDTH;
    int screenH = WINDOW_HEIGHT;

    Bonus heal;
    
    bool hit = false;
    Uint32 hitStart = 0;
    
    bool criticalActive = false;
    Uint32 criticalStart = 0;
    int criticalDamage = 0;
    int criticalX = 0;
    int criticalY = 0;
};

inline void initRandom() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

inline int randomInt(int max) {
    if (max <= 1) return 0;
    return std::rand() % max;
}

inline int randomInt(int min, int max) {
    if (max <= min) return min;
    return min + std::rand() % (max - min + 1);
}

void drawBonus(SDL_Renderer* renderer, const Bonus& b);
void trySpawnHeal(GameState& gs);
void checkHealPickup(GameState& gs);

