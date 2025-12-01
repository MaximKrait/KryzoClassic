#pragma once
#include <vector>
#include "utils.h"
#include "enemy.h"

void movePlayer(GameState& gs);
void respawnPlayer(GameState& gs);
bool handleCollisions(std::vector<Enemy>& enemies, GameState& gs);

