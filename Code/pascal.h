#pragma once
#include <vector>
#include "utils.h"
#include "enemy.h"

void movePlayer(GameState& gs, float deltaTime);
void respawnPlayer(GameState& gs);
bool handleCollisions(std::vector<Enemy>& enemies, GameState& gs);