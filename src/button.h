#pragma once

#include <raylib.h>

#define MAX_BUTTON_TEXT 10

typedef struct {
    Rectangle bounds;
    bool isHovered;
    bool isActive;
    char text[MAX_BUTTON_TEXT];
} Button;

void InitButton(Button *b, const char *text);
