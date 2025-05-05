#include "player.h"

void InitPlayer(Player *p, int x, int y)
{
    p->x = x;
    p->y = y;
    p->direction = DIR_NONE;
}

const char *GetDirectionText(Direction dir)
{
    switch (dir) {
    case DIR_UP: return "UP";
    case DIR_DOWN: return "DOWN";
    case DIR_RIGHT: return "RIGHT";
    case DIR_LEFT: return "LEFT";
    case DIR_NONE: return "NONE";
    }
    return "";
}
