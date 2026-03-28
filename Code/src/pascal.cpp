#include "pascal.h"
#include "collision.h"
#include "utils.h"
#include "log.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

void respawnPlayer(GameState&, const std::vector<Enemy>&) {
    // respawn system (not used in current version)
}

void movePlayer(GameState& gs, float deltaTime) {
    const Uint8* keystate = SDL_GetKeyboardState(nullptr);

    struct Dir { int key, dx, dy; };
    Dir dirs[] = {
        { SDL_SCANCODE_W, 0, -1 },
        { SDL_SCANCODE_S, 0,  1 },
        { SDL_SCANCODE_A,-1, 0 },
        { SDL_SCANCODE_D, 1, 0 }
    };

    for (auto& d : dirs) {
        if (keystate[d.key]) {
            gs.playerPosX += d.dx * PLAYER_SPEED * deltaTime;
            gs.playerPosY += d.dy * PLAYER_SPEED * deltaTime;
        }
    }

    gs.playerPosX = std::clamp(gs.playerPosX, 0.0f, float(gs.screenW - PLAYER_SIZE));
    gs.playerPosY = std::clamp(gs.playerPosY, 0.0f, float(gs.screenH - PLAYER_SIZE));
    gs.playerX = static_cast<int>(gs.playerPosX);
    gs.playerY = static_cast<int>(gs.playerPosY);
}

bool handleCollisions(std::vector<Enemy>& enemies, GameState& gs) {
    Uint32 now = SDL_GetTicks();

    for (auto& enemy : enemies) {
        if (!enemy.active || enemy.respawning) continue;

        float eRad = ENEMY_SIZE / 1.2f;
        float cx = gs.playerX + PLAYER_SIZE / 2.0f;
        float cy = gs.playerY + PLAYER_SIZE / 2.0f;

        float vx[3], vy[3];
        for (int i = 0; i < 3; i++) {
            float t = enemy.angle + i * (2 * M_PI / 3);
            vx[i] = enemy.x + ENEMY_SIZE / 2.0f + std::cos(t) * eRad;
            vy[i] = enemy.y + ENEMY_SIZE / 2.0f + std::sin(t) * eRad;
        }

        if (circleTriangleCollision(cx, cy, PLAYER_SIZE / 2.0f, vx[0], vy[0], vx[1], vy[1], vx[2], vy[2])) {
            int damage = randomInt(11) + 10;
            gs.hp -= damage;
            if (gs.hp < 0) gs.hp = 0;

            LOG("Player hit by enemy at (" + std::to_string(enemy.x) + ", " + std::to_string(enemy.y) + ") for " + std::to_string(damage) + " damage. HP now: " + std::to_string(gs.hp));

            if (damage == 20) {
                gs.criticalActive = true;
                gs.criticalStart = now;
                gs.criticalDamage = damage;
                gs.criticalX = gs.playerX;
                gs.criticalY = gs.playerY - 30;
                LOG("Critical hit! Damage: " + std::to_string(damage));
            }

            if (gs.hp <= 10 && gs.hpCriticalStart == 0) {
                gs.hpCriticalStart = now;
                LOG("HP critical threshold reached");
            } else if (gs.hp > 10) {
                gs.hpCriticalStart = 0;
            }

            gs.hit = true;
            gs.hitStart = now;

            float dx = gs.playerX - enemy.x;
            float dy = gs.playerY - enemy.y;
            float len = std::sqrt(dx*dx + dy*dy);
            if (len < 1) len = 1;
            dx /= len;
            dy /= len;

            gs.playerPosX += dx * 60.0f;
            gs.playerPosY += dy * 60.0f;

            gs.playerPosX = std::clamp(gs.playerPosX, 0.0f, float(gs.screenW - PLAYER_SIZE));
            gs.playerPosY = std::clamp(gs.playerPosY, 0.0f, float(gs.screenH - PLAYER_SIZE));
            gs.playerX = static_cast<int>(gs.playerPosX);
            gs.playerY = static_cast<int>(gs.playerPosY);

            if (gs.hp <= 0) {
                return true; // Game Over
            }

            break;
        }
    }

    if (gs.criticalActive && SDL_GetTicks() - gs.criticalStart > 600) {
        gs.criticalActive = false;
    }

    return false;
}