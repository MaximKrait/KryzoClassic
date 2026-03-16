#include "pascal.h"
#include <SDL2/SDL.h>
#include "utils.h"
#include <algorithm>
#include <cmath>

static bool intersects(int ax, int ay, int as, int bx, int by, int bs) {
    return ax < bx + bs &&
           ax + as > bx &&
           ay < by + bs &&
           ay + as > by;
}

void respawnPlayer(GameState& gs, const std::vector<Enemy>& enemies) {
    for (int i = 0; i < 50; i++) {
        int px = randomInt(std::max(1, gs.screenW - PLAYER_SIZE));
        int py = randomInt(std::max(1, gs.screenH - PLAYER_SIZE));

        bool ok = true;
        for (auto& e : enemies) {
            if (!e.active) continue;
            if (intersects(px, py, PLAYER_SIZE, (int)e.x, (int)e.y, ENEMY_SIZE)) {
                ok = false;
                break;
            }
        }
        if (ok) {
            gs.playerPosX = static_cast<float>(px);
            gs.playerPosY = static_cast<float>(py);
            gs.playerX = px;
            gs.playerY = py;
            return;
        }
    }


    gs.playerPosX = gs.screenW / 2.0f;
    gs.playerPosY = gs.screenH / 2.0f;
    gs.playerX = static_cast<int>(gs.playerPosX);
    gs.playerY = static_cast<int>(gs.playerPosY);
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
        
        if (gs.playerX < enemy.x + ENEMY_SIZE &&
            gs.playerX + PLAYER_SIZE > enemy.x &&
            gs.playerY < enemy.y + ENEMY_SIZE &&
            gs.playerY + PLAYER_SIZE > enemy.y) {

            int damage = randomInt(11) + 10;
            gs.hp -= damage;
            if (gs.hp < 0) gs.hp = 0;

            if (damage == 20) {
                gs.criticalActive = true;
                gs.criticalStart = now;
                gs.criticalDamage = damage;
                gs.criticalX = gs.playerX;
                gs.criticalY = gs.playerY - 30;
            }

            if (gs.hp <= 10 && gs.hpCriticalStart == 0) {
                gs.hpCriticalStart = now;
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
