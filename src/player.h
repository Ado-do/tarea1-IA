#pragma once

typedef enum { DIR_NONE, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;

typedef struct {
    int x, y;
    Direction direction;
} Player;

void InitPlayer(Player *p, int x, int y);
const char *GetDirectionText(Direction dir);
