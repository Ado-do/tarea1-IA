#include "render.h"
#include "search.h"
#include "update.h"
#include <math.h>
#include <raylib.h>
#include <stddef.h>

void DrawButton(Button *button, Color normalColor, Color hoverColor, Color activeColor, float fontSize)
{
    Color color = button->isActive ? activeColor : (button->isHovered ? hoverColor : normalColor);

    // Draw rectangle
    DrawRectangleLinesEx(button->bounds, 2, color);

    // Draw centered text
    Vector2 textSize = MeasureTextEx(GetFontDefault(), button->text, fontSize, 1);
    float textX = button->bounds.x + (button->bounds.width - textSize.x) * 0.5f;
    float textY = button->bounds.y + (button->bounds.height - textSize.y) * 0.5f;

    DrawText(button->text, textX, textY, fontSize, color);
}

void DrawLeftAlignedText(const char *t, float x, float y, float fontSize, Color color)
{
    float w = MeasureText(t, fontSize);
    for (unsigned int i = 0; i < TextLength(t); i++) {
        if (t[i] == '1')
            w += fontSize * 0.2f;
    }
    DrawText(t, x - w, y, fontSize, color);
}

void DrawCenteredText(const char *t, float x, float y, float fontSize, Color color)
{
    float w = MeasureText(t, fontSize);
    if (TextLength(t) >= 2 && t[0] == '1' && t[1] == '\0')
        w += fontSize * 0.3f;

    DrawText(t, x - w * 0.5f, y - fontSize * 0.5f, fontSize, color);
}

void DrawTitle(Game *g)
{
    const int sw = GetScreenWidth(), sh = GetScreenHeight();

    static float flashTimer_s = 0.0f;
    const float flashInterval_s = 0.75f;
    static bool visibleFlash = false;

    flashTimer_s += GetFrameTime();
    if (flashTimer_s >= flashInterval_s) {
        flashTimer_s = 0.0f;
        visibleFlash = !visibleFlash;
    }

    ClearBackground(BLACK);

    float titleSize = sh * UI_TITLE_SCALE;
    DrawCenteredText(GAME_NAME, sw * 0.5f - 5, sh * 0.45f - 5, titleSize, DARKGRAY);
    DrawCenteredText(GAME_NAME, sw * 0.5f, sh * 0.45f, titleSize, WHITE);

    if (visibleFlash) {
        float promptSize = sh * 0.06f;
        DrawCenteredText("Press any key to continue", sw * 0.5f, sh * 0.63f, promptSize, GRAY);
    }

    float infoSize = sh * UI_INFO_SCALE;
    DrawLeftAlignedText(TextFormat("mazes loaded from input: %d", g->totalScreens), sw - UI_PADDING, sh * 0.95f, infoSize, DARKGRAY);
}

static void DrawPlayer(const Player *p, Rectangle mazeRec, float cellSize)
{
    float playerSize = cellSize * 0.4f;
    Color playerColor = GREEN;
    Rectangle playerCell = {mazeRec.x + (p->x * cellSize), mazeRec.y + (p->y * cellSize), cellSize, cellSize};
    Rectangle playerRec = {playerCell.x + (cellSize - playerSize) * 0.5f, playerCell.y + (cellSize - playerSize) * 0.5f, playerSize, playerSize};

    DrawRectangleRec(playerRec, playerColor);
    DrawRectangleLinesEx(playerRec, 2, ColorBrightness(playerColor, 0.6f));

    if (p->direction != DIR_NONE) {
        const float dirMargin = cellSize * 0.1f;
        const float dirSize = cellSize * 0.1f;
        Vector2 dirPointer = {0};
        Vector2 v1 = {0}, v2 = {0};

        switch (p->direction) {
            case DIR_UP:
                dirPointer = (Vector2){playerCell.x + (cellSize * 0.5f), playerCell.y + dirMargin};
                v1 = (Vector2){dirPointer.x - dirSize, dirPointer.y + dirSize};
                v2 = (Vector2){dirPointer.x + dirSize, dirPointer.y + dirSize};
                break;
            case DIR_DOWN:
                dirPointer = (Vector2){playerCell.x + (cellSize * 0.5f), playerCell.y + cellSize - dirMargin};
                v1 = (Vector2){dirPointer.x + dirSize, dirPointer.y - dirSize};
                v2 = (Vector2){dirPointer.x - dirSize, dirPointer.y - dirSize};
                break;
            case DIR_RIGHT:
                dirPointer = (Vector2){playerCell.x + cellSize - dirMargin, playerCell.y + (cellSize * 0.5f)};
                v1 = (Vector2){dirPointer.x - dirSize, dirPointer.y - dirSize};
                v2 = (Vector2){dirPointer.x - dirSize, dirPointer.y + dirSize};
                break;
            case DIR_LEFT:
                dirPointer = (Vector2){playerCell.x + dirMargin, playerCell.y + (cellSize * 0.5f)};
                v1 = (Vector2){dirPointer.x + dirSize, dirPointer.y + dirSize};
                v2 = (Vector2){dirPointer.x + dirSize, dirPointer.y - dirSize};
                break;
            default: break;
        }

        if (p->direction != DIR_NONE) {
            DrawTriangle(dirPointer, v1, v2, playerColor);
        }
    }
}

static void DrawInfo(const Game *g, Rectangle rec)
{
    Maze *m = GetCurrentMaze(g);
    Player *p = GetCurrentPlayer(g);
    SearchData *sd = GetCurrentSearchData(g);

    const float infoSize = rec.height * UI_INFO_SCALE;
    Vector2 infoPos = {rec.x + UI_PADDING, rec.y + UI_PADDING};
    Vector2 infoPad = {10.0f, 28.0f};
    Color infoColor = WHITE;

    const char *text = TextFormat("Rows = %d, Cols = %d\n"
                                  "Start = (%d, %d), Goal = (%d, %d)\n\n"
                                  "Player Position = (%d, %d)\n"
                                  "Player Direction = %s\n\n"
                                  "Search Type = %s\n"
                                  "Search State = %s\n"
                                  "Steps Count = %u\n"
                                  "Final Path Steps = %u",
                                  m->rows, m->cols, m->start.x, m->start.y, m->goal.x, m->goal.y, p->x, p->y, GetDirectionText(p->direction),
                                  GetSearchTypeText(sd->type), GetSearchStateText(sd->state), sd->stepCount, sd->finalPathLength);

    DrawText("Current Maze Info:", infoPos.x, infoPos.y, infoSize + 4, infoColor);
    DrawText(text, infoPos.x + infoPad.x, infoPos.y + infoPad.y, infoSize, infoColor);
}

static void DrawVelocity(Game *g, Rectangle rec)
{
    const float velSize = rec.height * UI_VEL_SCALE;
    const char *velText = TextFormat("%.2fx", GetCurrentVelocity(g));
    const float velWidth = MeasureText("3.00x", velSize); // Consistent width
    const float velPad = UI_PADDING;

    float x = rec.x + rec.width - 2 * UI_PADDING;
    float y = rec.y + rec.height - velSize;
    Vector2 buttonDim = {rec.width * UI_BUTTON_SCALE, velSize};

    g->addVelButton.bounds = (Rectangle){x - buttonDim.x, y - (buttonDim.y * 0.5f), buttonDim.x, buttonDim.y};

    Rectangle velRec = {g->addVelButton.bounds.x - velWidth - velPad, y - (velSize * 0.5f), velWidth, velSize};

    g->rmVelButton.bounds = (Rectangle){velRec.x - buttonDim.x - velPad, y - (velSize * 0.5f), buttonDim.x, buttonDim.y};

    DrawRectangleRec(g->addVelButton.bounds, GREEN);
    DrawButton(&g->addVelButton, BLACK, WHITE, BLACK, velSize);

    DrawText(velText, velRec.x, velRec.y, velSize, WHITE);

    DrawRectangleRec(g->rmVelButton.bounds, GREEN);
    DrawButton(&g->rmVelButton, BLACK, WHITE, BLACK, velSize);
}

static void DrawControls(Rectangle rec)
{
    const float controlsSize = rec.height * UI_INFO_SCALE;
    Vector2 controlsPos = {rec.x + UI_PADDING, rec.y + (rec.height * 0.55f) + UI_PADDING};
    Vector2 controlsPad = {10.0f, 28.0f};
    Color controlsColor = WHITE;

    const char *text = "- Main action (solve, jump, next):\n  Space key\n"
                       "- Reset Maze: R key\n"
                       "- Change between mazes:\n  Number keys (1 - ... - 9 - 0)\n"
                       "- Change between search types:\n  [B]FS  [D]FS [U]CS  [A]*  [M]ANUAL\n"
                       "- Move player (Manual Search):\n  Arrow keys and Space key\n"
                       "- Change velocity:\n  +/- keys";

    DrawText("Keyboard Controls:", controlsPos.x, controlsPos.y, controlsSize + 4, controlsColor);
    DrawText(text, controlsPos.x + controlsPad.x, controlsPos.y + controlsPad.y, controlsSize, controlsColor);
}

static void DrawMaze(Game *g, Rectangle rec)
{
    const Color defaultNodeColor = DARKGRAY;
    const Color visitedNodeColor = BLACK;
    const Color pathNodeColor = DARKGREEN;
    const Color goalNotFoundColor = RED;

    const Maze *m = GetCurrentMaze(g);
    const SearchData *sd = GetCurrentSearchData(g);
    const Player *p = GetCurrentPlayer(g);

    float cellSize = fminf((rec.width * 0.9f) / m->cols, (rec.height * 0.9f) / m->rows);

    float numbersSize = cellSize * 0.25f;
    Rectangle mazeRec = {rec.x + (rec.width - (cellSize * m->cols)) * 0.5f, rec.y + (rec.height - (cellSize * m->rows)) * 0.5f, cellSize * m->cols,
                         cellSize * m->rows};

    for (int y = 0; y < m->rows; y++) {
        for (int x = 0; x < m->cols; x++) {
            Rectangle cellRec = {mazeRec.x + (x * cellSize), mazeRec.y + (y * cellSize), cellSize, cellSize};

            bool isStart = (x == m->start.x && y == m->start.y);
            bool isGoal = IsGoalPosition(m, x, y);

            Color cellColor = defaultNodeColor;

            if (IsNodeInFinalPath(sd, x, y)) {
                cellColor = pathNodeColor;
            } else if (IsNodeVisited(sd, x, y)) {
                if (sd->type == MANUAL && sd->state == FOUND) {
                    cellColor = pathNodeColor;
                } else {
                    cellColor = visitedNodeColor;
                }
            }

            if (isGoal && sd->state == NOT_FOUND) {
                cellColor = goalNotFoundColor;
            }

            DrawRectangleRec(cellRec, cellColor);
            DrawRectangleLinesEx(cellRec, 2, RAYWHITE);

            DrawLeftAlignedText(TextFormat("%d", m->grid[y][x]), cellRec.x + cellSize - 6, cellRec.y + 4, numbersSize, RAYWHITE);

#ifdef DEBUG
            // Draw position coordinates in debug mode
            float positionSize = cellSize * 0.12f;
            DrawLeftAlignedText(TextFormat("(%d, %d)", x, y), cellRec.x + cellSize - 4, cellRec.y + cellSize - positionSize - 5, positionSize, WHITE);
#endif

            if (isStart) {
                DrawText("[S]", cellRec.x + 6, cellRec.y + 5, numbersSize, GREEN);
            }
            if (isGoal) {
                DrawText("[G]", cellRec.x + 6, cellRec.y + 5, numbersSize, GREEN);
            }
        }
    }

    DrawPlayer(p, mazeRec, cellSize);
    DrawInfo(g, rec);
    DrawVelocity(g, rec);
    if (g->showControls)
        DrawControls(rec);
    else
        DrawText("Show/Hide controls: C key\n", 10, rec.y + rec.height - 24, 20, WHITE);
}

static void DrawTopLine(Game *g, Rectangle rec)
{
    const SearchData *sd = GetCurrentSearchData(g);

    // Draw top bar background
    DrawRectangleRec(rec, DARKGRAY);
    DrawRectangleLinesEx(rec, 1, BLACK);

    float topFontSize = rec.height * 0.5f;
    float buttonSize = rec.height * 0.7f;
    float buttonPad = UI_PADDING;
    float searchMargin = 20;
    float x_mid = rec.x + (rec.width * 0.5f);
    float y_mid = rec.y + (rec.height * 0.5f);

    // Left
    // DrawText(TextFormat("Maze %d", (g->currentScreen == 9) ? 0 : g->currentScreen + 1), rec.x + UI_PADDING, y - (topFontSize * 0.5f), topFontSize, WHITE);
    float x_search = UI_PADDING;
    for (int i = 0; i < 5; i++) {
        Button *curr = &g->searchButtons[i];
        curr->bounds = (Rectangle){x_search, y_mid - (buttonSize * 0.5f), MeasureText(curr->text, topFontSize) + searchMargin, buttonSize};

        curr->isActive = (i == (int)sd->type);
        DrawRectangleRec(curr->bounds, curr->isActive ? GREEN : GRAY);
        DrawButton(curr, BLACK, WHITE, BLACK, topFontSize);

        x_search += curr->bounds.width + buttonPad;
    }

    // Rigth
    float numberSize = buttonSize * 0.8f;
    float numberMargin = 10.0f;
    Rectangle buttonRec = {rec.x + rec.width, (rec.height - buttonSize) * 0.5f, buttonSize, buttonSize};

    for (int i = g->totalScreens - 1; i >= 0; i--) {
        buttonRec.x -= ((i == g->totalScreens - 1) ? UI_PADDING : numberMargin) + buttonSize;

        g->mazeButtons[i].bounds = buttonRec;
        g->mazeButtons[i].isActive = (i == g->currentScreen);

        DrawRectangleRec(buttonRec, g->mazeButtons[i].isActive ? GREEN : GRAY);
        DrawButton(&g->mazeButtons[i], BLACK, WHITE, BLACK, numberSize);
    }


    // Middle
    const char *mainText;
    if (sd->type == MANUAL) {
        mainText = (sd->state == FOUND) ? "NEXT" : "JUMP";
    } else {
        switch (sd->state) {
            case NOT_SEARCHING: mainText = "SOLVE"; break;
            case SEARCHING: mainText = "PAUSE"; break;
            case FOUND:
            case NOT_FOUND: mainText = "NEXT"; break;
        }
    }
    TextCopy(g->actionButton.text, mainText);

    const float mainWidth = fmaxf(MeasureText("SOLVE", topFontSize), fmaxf(MeasureText("PAUSE", topFontSize), MeasureText("RESET", topFontSize)));
    const float recPad = 1.4f;

    Rectangle mainRec = {x_mid - (mainWidth * 0.5f * recPad), y_mid - (topFontSize * 0.5f * recPad), mainWidth * recPad, topFontSize * recPad};

    g->actionButton.bounds = mainRec;

    DrawRectangleRec(mainRec, GREEN);
    DrawButton(&g->actionButton, BLACK, WHITE, BLACK, topFontSize);
}

void DrawGameplay(Game *g)
{
    ClearBackground(COLOR_BACKGROUND);

    // Calculate layout
    const int sw = GetScreenWidth(), sh = GetScreenHeight();
    Rectangle topLine = {0, 0, sw, sh * 0.075f};
    Rectangle mazeScreen = {0, topLine.height, sw, sh - topLine.height};

    DrawTopLine(g, topLine);
    DrawMaze(g, mazeScreen);
}
