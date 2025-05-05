#include "button.h"

void InitButton(Button *b, const char *text)
{
    *b = (Button){
        .bounds = {0},
        .isActive = false,
        .isHovered = false,
    };
    TextCopy(b->text, text);
}
