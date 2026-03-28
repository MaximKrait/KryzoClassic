#include "enemy.h"
#include "collision.h"
#include "utils.h"
#include "log.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <algorithm>

static void safeEnemyRespawn(Enemy& e, const GameState& gs, const std::vector<Enemy>& list) {
    for (int i = 0; i < 50; i++) {
        int rx = randomInt(std::max(1, gs.screenW - ENEMY_SIZE));
        int ry = randomInt(std::max(1, gs.screenH - ENEMY_SIZE));

        bool ok = true;

        float cx = gs.playerX + PLAYER_SIZE/2.0f;
        float cy = gs.playerY + PLAYER_SIZE/2.0f;
        float radius = PLAYER_SIZE/2.0f;

        float eRad = ENEMY_SIZE / 1.2f;
        float vx[3], vy[3];
        for (int j = 0; j < 3; j++) {
            float t = j * (2*M_PI/3);
            vx[j] = rx + ENEMY_SIZE/2.0f + std::cos(t)*eRad;
            vy[j] = ry + ENEMY_SIZE/2.0f + std::sin(t)*eRad;
        }

        if (circleTriangleCollision(cx, cy, radius, vx[0], vy[0], vx[1], vy[1], vx[2], vy[2]))
            ok = false;

        if (ok) {
            for (auto& other : list) {
                if (!other.active || &other == &e) continue;
                for (int j = 0; j < 3; j++) {
                    float t = other.angle + j*(2*M_PI/3);
                    vx[j] = other.x + ENEMY_SIZE/2.0f + std::cos(t)*eRad;
                    vy[j] = other.y + ENEMY_SIZE/2.0f + std::sin(t)*eRad;
                }
                if (circleTriangleCollision(rx + ENEMY_SIZE/2.0f, ry + ENEMY_SIZE/2.0f, eRad,
                                            vx[0], vy[0], vx[1], vy[1], vx[2], vy[2])) {
                    ok = false;
                    break;
                }
            }
        }

        if (ok) {
            e.respawnX = rx;
            e.respawnY = ry;
            LOG("Enemy safe respawn set at (" + std::to_string(rx) + ", " + std::to_string(ry) + ")");
            return;
        }
    }

    e.respawnX = gs.screenW / 2;
    e.respawnY = gs.screenH / 2;
    LOG("Enemy safe respawn fallback to center (" + std::to_string(e.respawnX) + ", " + std::to_string(e.respawnY) + ")");
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
        LOG("Enemy initialized at (" + std::to_string(e.x) + ", " + std::to_string(e.y) + ")");
    }
}

static void applyDifficultyScaling(Enemy& e, const GameState& gs) {
    int oldSpeed = e.currentSpeed;
    int oldDash = e.currentDash;

    int score = gs.score;
    if (score < 50) {
        e.currentSpeed = BASE_SPEED;
        e.currentDash = DASH_SPEED;
        e.dashEnabled = false;
    } else if (score < 100) {
        e.currentSpeed = BASE_SPEED + 1;
        e.currentDash = DASH_SPEED;
        e.dashEnabled = true;
        e.dashType = DashType::FORWARD;
        e.dashCooldownRange = {DASH_COOLDOWN_MAX, DASH_COOLDOWN_MAX};
    } else if (score < 150) {
        e.currentSpeed = BASE_SPEED + 1;
        e.currentDash = DASH_SPEED;
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

    if (oldSpeed != e.currentSpeed || oldDash != e.currentDash) {
        LOG("Enemy difficulty scaled: speed=" + std::to_string(e.currentSpeed) + 
            ", dash=" + std::to_string(e.currentDash));
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
        e.dashVX = std::cos(e.angle) * e.currentDash;
        e.dashVY = std::sin(e.angle) * e.currentDash;
    } else if (e.dashType == DashType::TARGET) {
        if (dist < 0.01f) return;
        e.dashVX = (dx / dist) * e.currentDash;
        e.dashVY = (dy / dist) * e.currentDash;
        e.angle = std::atan2(dy, dx);
    }

    e.dashing = true;
    e.dashStart = now;
    e.dashCooldown = now + randomInt(e.dashCooldownRange.first, e.dashCooldownRange.second);
    LOG("Enemy started dash type=" + std::to_string(int(e.dashType)) +
        " vx=" + std::to_string(e.dashVX) + " vy=" + std::to_string(e.dashVY) +
        " cooldown until " + std::to_string(e.dashCooldown));
}

void updateEnemies(std::vector<Enemy>& enemies, const GameState& gs, float deltaTime) {
    Uint32 now = SDL_GetTicks();

    for (auto& e : enemies) {
        if (!e.active) continue;
        if (e.respawning) {
            if (now - e.respawnStart >= 2000) {
                e.respawning = false;
                e.x = e.respawnX;
                e.y = e.respawnY;
                LOG("Enemy finished respawning at (" + std::to_string(e.x) + ", " + std::to_string(e.y) + ")");
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
                        e.dashVX = (dx / dist) * e.currentDash;
                        e.dashVY = (dy / dist) * e.currentDash;
                        e.angle = std::atan2(dy, dx);
                    }
                }
                e.x += e.dashVX * deltaTime;
                e.y += e.dashVY * deltaTime;
            } else {
                e.dashing = false;
                LOG("Enemy finished dash at (" + std::to_string(e.x) + ", " + std::to_string(e.y) + ")");
            }
        } else {
            float dx = (gs.playerX + PLAYER_SIZE/2.0f) - (e.x + ENEMY_SIZE/2.0f);
            float dy = (gs.playerY + PLAYER_SIZE/2.0f) - (e.y + ENEMY_SIZE/2.0f);
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist > 0.01f) {
                e.x += (dx / dist) * e.currentSpeed * deltaTime;
                e.y += (dy / dist) * e.currentSpeed * deltaTime;
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
                    LOG("Enemy collision triggered respawn at (" +
                        std::to_string(enemies[j].respawnX) + ", " + std::to_string(enemies[j].respawnY) + ")");
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