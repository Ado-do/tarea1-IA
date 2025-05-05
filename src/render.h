#pragma once

#include "game.h"
#include <raylib.h>

#define UI_PADDING 10.0f
#define UI_BUTTON_PADDING 5.0f
#define UI_TITLE_SCALE 0.2f
#define UI_INFO_SCALE 0.03f
#define UI_VEL_SCALE 0.04f
#define UI_BUTTON_SCALE 0.03f

#define COLOR_BACKGROUND (Color){40, 40, 40, 255}
#define COLOR_MIDGRAY (Color){120, 120, 120, 255}
#define COLOR_BUTTON_NORMAL DARKGRAY
#define COLOR_BUTTON_HOVER WHITE
#define COLOR_BUTTON_ACTIVE BLACK

void DrawTitle(Game *g);
void DrawGameplay(Game *g);
