#include "enemy.h"
#include "utils.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <algorithm>

static bool intersectsXY(float ax, float ay, float as, float bx, float by, float bs) {
    return ax < bx + bs &&
           ax + as > bx &&
           ay < by + bs &&
           ay + as > by;
}

static void safeEnemyRespawn(Enemy& e, const GameState& gs, const std::vector<Enemy>& list) {
    for (int i = 0; i < 50; i++) {
        int rx = randomInt(std::max(1, gs.screenW - ENEMY_SIZE));
        int ry = randomInt(std::max(1, gs.screenH - ENEMY_SIZE));

        bool ok = true;

        if (intersectsXY(rx, ry, ENEMY_SIZE, gs.playerX, gs.playerY, PLAYER_SIZE))
            ok = false;

        if (ok) {
            for (auto& other : list) {
                if (!other.active || &other == &e) continue;
                if (intersectsXY(rx, ry, ENEMY_SIZE, other.x, other.y, ENEMY_SIZE)) {
                    ok = false;
                    break;
                }
            }
        }

        if (ok) {
            e.respawnX = rx;
            e.respawnY = ry;
            return;
        }
    }

    e.respawnX = gs.screenW / 2;
    e.respawnY = gs.screenH / 2;
}

void initEnemies(std::vector<Enemy>& enemies, int num, int screenW, int screenH) {
    enemies.clear();
    enemies.resize(num);

    for (auto& e : enemies) {
        e.x = randomInt(std::max(1, screenW - ENEMY_SIZE));
        e.y = randomInt(std::max(1, screenH - ENEMY_SIZE));
        e.angle = 0;
        e.active = true;
        e.dashing = false;
        e.dashCooldown = 0;
        e.dashType = DashType::NONE;
    }
}

static void applyDifficultyScaling(Enemy& e, const GameState& gs) {
    int score = gs.score;
    if (score < 50) {
        e.currentSpeed = BASE_SPEED;
        e.dashEnabled = false;
    } else if (score < 100) {
        e.currentSpeed = BASE_SPEED + 1;
        e.dashEnabled = true;
        e.dashType = DashType::FORWARD;
        e.dashCooldownRange = {DASH_COOLDOWN_MAX, DASH_COOLDOWN_MAX};
    } else if (score < 150) {
        e.currentSpeed = BASE_SPEED + 1;
        e.dashEnabled = true;
        e.dashType = DashType::FORWARD;
        e.dashCooldownRange = {DASH_COOLDOWN_MAX - 500, DASH_COOLDOWN_MAX - 200};
    } else {
        e.currentSpeed = BASE_SPEED + 1;
        e.currentDash = DASH_SPEED - 2;
        e.dashEnabled = true;
        e.dashType = DashType::TARGET;
        e.dashCooldownRange = {DASH_COOLDOWN_MIN, DASH_COOLDOWN_MAX};
    }
}

static void tryStartDash(Enemy& e, const GameState& gs) {
    if (!e.dashEnabled) return;

    Uint32 now = SDL_GetTicks();
    if (now < e.dashCooldown) return;

    if (randomInt(200) != 0) return;

    if (e.dashType == DashType::NONE)
        e.dashType = (randomInt(2) == 0 ? DashType::FORWARD : DashType::TARGET);

    float dx = (gs.playerX + PLAYER_SIZE/2.0f) - (e.x + ENEMY_SIZE/2.0f);
    float dy = (gs.playerY + PLAYER_SIZE/2.0f) - (e.y + ENEMY_SIZE/2.0f);
    float dist = std::sqrt(dx*dx + dy*dy);

    if (e.dashType == DashType::FORWARD) {
        e.dashVX = std::cos(e.angle) * DASH_SPEED;
        e.dashVY = std::sin(e.angle) * DASH_SPEED;
    } else if (e.dashType == DashType::TARGET) {
        if (dist < 0.01f) return;
        e.dashVX = (dx / dist) * DASH_SPEED;
        e.dashVY = (dy / dist) * DASH_SPEED;
        e.angle = std::atan2(dy, dx);
    }

    e.dashing = true;
    e.dashStart = now;
    e.dashCooldown = now + randomInt(e.dashCooldownRange.first, e.dashCooldownRange.second);
}

void updateEnemies(std::vector<Enemy>& enemies, const GameState& gs) {
    Uint32 now = SDL_GetTicks();

    for (auto& e : enemies) {
        if (!e.active) continue;

        if (e.respawning) {
            if (now - e.respawnStart >= 2000) {
                e.respawning = false;
                e.x = e.respawnX;
                e.y = e.respawnY;
            }
            continue;
        }

        applyDifficultyScaling(e, gs);

        if (e.dashing) {
            if (now - e.dashStart <= DASH_DURATION) {
                if (e.dashType == DashType::TARGET) {
                    float dx = (gs.playerX + PLAYER_SIZE/2.0f) - (e.x + ENEMY_SIZE/2.0f);
                    float dy = (gs.playerY + PLAYER_SIZE/2.0f) - (e.y + ENEMY_SIZE/2.0f);
                    float dist = std::sqrt(dx*dx + dy*dy);
                    if (dist > 0.01f) {
                        e.dashVX = (dx / dist) * DASH_SPEED;
                        e.dashVY = (dy / dist) * DASH_SPEED;
                        e.angle = std::atan2(dy, dx);
                    }
                }
                e.x += e.dashVX;
                e.y += e.dashVY;
            } else {
                e.dashing = false;
            }
        } else {
            float dx = (gs.playerX + PLAYER_SIZE/2.0f) - (e.x + ENEMY_SIZE/2.0f);
            float dy = (gs.playerY + PLAYER_SIZE/2.0f) - (e.y + ENEMY_SIZE/2.0f);
            float dist = std::sqrt(dx*dx + dy*dy);

            if (dist > 0.01f) {
                e.x += (dx / dist) * e.currentSpeed;
                e.y += (dy / dist) * e.currentSpeed;
                e.angle = std::atan2(dy, dx);
            }

            tryStartDash(e, gs);
        }

        e.x = std::clamp(e.x, 0.0f, float(gs.screenW - ENEMY_SIZE));
        e.y = std::clamp(e.y, 0.0f, float(gs.screenH - ENEMY_SIZE));
    }

    for (size_t i = 0; i < enemies.size(); i++) {
        for (size_t j = i + 1; j < enemies.size(); j++) {
            if (!enemies[i].active || !enemies[j].active) continue;

            if (std::abs(enemies[i].x - enemies[j].x) < ENEMY_SIZE &&
                std::abs(enemies[i].y - enemies[j].y) < ENEMY_SIZE) {

                if (!enemies[j].respawning) {
                    enemies[j].respawning = true;
                    enemies[j].respawnStart = now;
                    safeEnemyRespawn(enemies[j], gs, enemies);
                }
            }
        }
    }
}

void renderEnemies(const std::vector<Enemy>& enemies, SDL_Renderer* r, const GameState& gs) {
    for (auto& e : enemies) {
        float cx = e.x + ENEMY_SIZE/2.0f;
        float cy = e.y + ENEMY_SIZE/2.0f;
        float rad = ENEMY_SIZE / 1.2f;
        Sint16 vx[3], vy[3];

        if (e.respawning) {
            cx = e.respawnX + ENEMY_SIZE/2.0f;
            cy = e.respawnY + ENEMY_SIZE/2.0f;
            float ang = 0;
            for (int i = 0; i < 3; i++) {
                float t = ang + i * (2*M_PI/3);
                vx[i] = cx + std::cos(t)*rad;
                vy[i] = cy + std::sin(t)*rad;
            }
            filledPolygonRGBA(r, vx, vy, 3, 255,255,0,128);
            continue;
        }

        float ang = e.angle;
        for (int i = 0; i < 3; i++) {
            float t = ang + i*(2*M_PI/3);
            vx[i] = cx + std::cos(t)*rad;
            vy[i] = cy + std::sin(t)*rad;
        }

        if (e.dashing)
            filledPolygonRGBA(r, vx, vy, 3, 255,128,0,255);
        else
            filledPolygonRGBA(r, vx, vy, 3, 255,0,0,255);
    }
}

