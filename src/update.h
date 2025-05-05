#pragma once

#define DEFAULT_VELOCITY 3

#include "game.h"

void UpdateTitle(Game *g);
void UpdateGameplay(Game *g);

float GetCurrentVelocity(const Game *g);
