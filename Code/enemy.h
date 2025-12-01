#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include "utils.h"
#include <utility>

inline constexpr float BASE_SPEED = 2.0f;
inline constexpr float DASH_SPEED = 7.0f;
inline constexpr Uint32 DASH_DURATION = 700;
inline constexpr Uint32 DASH_COOLDOWN_MIN = 1500;
inline constexpr Uint32 DASH_COOLDOWN_MAX = 3000;

enum class DashType {
    NONE = 0,
    FORWARD,
    TARGET
};

struct Enemy {
    float x = 0;
    float y = 0;
    bool active = true;

    float angle = 0;

    float respawnX = 0;
    float respawnY = 0;
    bool respawning = false;
    Uint32 respawnStart = 0;

    bool dashing = false;
    float dashVX = 0;
    float dashVY = 0;
    Uint32 dashStart = 0;
    Uint32 dashCooldown = 0;
    DashType dashType = DashType::NONE;

    float currentSpeed = BASE_SPEED;
    float currentDash = DASH_SPEED;
    bool dashEnabled = false;
    std::pair<Uint32, Uint32> dashCooldownRange = {DASH_COOLDOWN_MIN, DASH_COOLDOWN_MAX};
};

void initEnemies(std::vector<Enemy>& enemies, int num, int screenW, int screenH);
void updateEnemies(std::vector<Enemy>& enemies, const GameState& gs);
void renderEnemies(const std::vector<Enemy>& enemies, SDL_Renderer* renderer, const GameState& gs);

