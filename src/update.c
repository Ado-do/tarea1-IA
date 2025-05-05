#include "update.h"
#include "game.h"
#include "maze.h"
#include "search.h"

#include <raylib.h>
#include <raymath.h>

static const float VELOCITY_PRESETS[] = {0.5f, 0.75f, 1.0f, 1.5f, 2.0f, 3.0f};
#define NUM_VELOCITY_PRESETS (sizeof(VELOCITY_PRESETS) / sizeof(VELOCITY_PRESETS[0]))

static bool IsButtonPressed(Button *button)
{
    return CheckCollisionPointRec(GetMousePosition(), button->bounds) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

static bool UpdateButtonHoverState(Button *button)
{
    bool wasHovered = button->isHovered;
    button->isHovered = CheckCollisionPointRec(GetMousePosition(), button->bounds);
    return button->isHovered != wasHovered;
}

static void SetNextVelocity(Game *g)
{
    if (g->currentVelocity + 1 < (int)NUM_VELOCITY_PRESETS) {
        g->currentVelocity++;
    }
}

static void SetPrevVelocity(Game *g)
{
    if (g->currentVelocity > 0) {
        g->currentVelocity--;
    }
}

static void Jump(Game *g)
{
    Player *p = GetCurrentPlayer(g);
    SearchData *sd = GetCurrentSearchData(g);

    if (p->direction != DIR_NONE && ManualStep(sd, p->direction)) {
        if (sd->state == NOT_SEARCHING) {
            sd->state = SEARCHING;
        }
        p->x = sd->currentNode->x;
        p->y = sd->currentNode->y;
    }
    p->direction = DIR_NONE;
}

static void MainAction(Game *g)
{
    SearchData *sd = GetCurrentSearchData(g);
    Player *p = GetCurrentPlayer(g);
    Maze *m = GetCurrentMaze(g);

    if (sd->type == MANUAL) {
        if (sd->state != FOUND) {
            Jump(g);
        } else {
            InitPlayer(p, m->start.x, m->start.y);
            ResetSearchData(sd);
            sd->type = DFS;
            if (g->currentScreen < g->totalScreens - 1) {
                g->currentScreen++;
            } else {
                g->currentScreen = 0;
            }
        }
    } else {
        if (sd->state == SEARCHING) {
            sd->state = NOT_SEARCHING;
        } else if (sd->state == NOT_SEARCHING) {
            sd->state = SEARCHING;
        } else {
            sd->type++;
            InitPlayer(p, m->start.x, m->start.y);
            ResetSearchData(sd);
        }
    }
}


static void HandleMazeButtonPress(Game *g, int buttonIndex)
{
    SearchData *sd = GetCurrentSearchData(g);

    if (sd->state == FOUND || sd->state == NOT_FOUND)
        sd->state = NOT_SEARCHING;

    g->currentScreen = buttonIndex;

    Player *p = GetCurrentPlayer(g);
    Maze *m = GetCurrentMaze(g);
    InitPlayer(p, m->start.x, m->start.y);
    ResetSearchData(GetCurrentSearchData(g));
}

static void HandleMouseInput(Game *g)
{
    if (g->currentState != GAMEPLAY_STATE)
        return;

    for (int i = 0; i < g->totalScreens; i++) {
        if (IsButtonPressed(&g->mazeButtons[i])) {
            HandleMazeButtonPress(g, i);
            break;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (IsButtonPressed(&g->searchButtons[i])) {
            ChangeCurrentSearchType(g, i);
        }
    }

    if (IsButtonPressed(&g->actionButton)) {
        MainAction(g);
    }

    if (IsButtonPressed(&g->addVelButton)) {
        SetNextVelocity(g);
    }

    if (IsButtonPressed(&g->rmVelButton)) {
        SetPrevVelocity(g);
    }
}

static void UpdateButtonStates(Game *g)
{
    if (g->currentState != GAMEPLAY_STATE)
        return;

    for (int i = 0; i < g->totalScreens; i++) {
        UpdateButtonHoverState(&g->mazeButtons[i]);
    }

    for (int i = 0; i < 5; i++) {
        UpdateButtonHoverState(&g->searchButtons[i]);
    }

    UpdateButtonHoverState(&g->actionButton);
    UpdateButtonHoverState(&g->addVelButton);
    UpdateButtonHoverState(&g->rmVelButton);
}

static void HandleKeyInput(Game *g)
{
    const int key = GetKeyPressed();
    if (key == 0)
        return;

    SearchData *sd = GetCurrentSearchData(g);
    Player *p = GetCurrentPlayer(g);
    Maze *m = GetCurrentMaze(g);

    // Global controls
    switch (key) {
        case KEY_Q: g->isRunning = false; return;
        case KEY_SPACE: MainAction(g); break;
        case KEY_C: g->showControls = !g->showControls;
        case KEY_R:
            ResetSearchData(sd);
            InitPlayer(p, m->start.x, m->start.y);
            // g->currentVelocity = DEFAULT_VELOCITY;
            return;

        case KEY_M: ChangeCurrentSearchType(g, MANUAL); break;
        case KEY_D: ChangeCurrentSearchType(g, DFS); break;
        case KEY_B: ChangeCurrentSearchType(g, BFS); break;
        case KEY_U: ChangeCurrentSearchType(g, UCS); break;
        case KEY_A: ChangeCurrentSearchType(g, A_START); break;

        case KEY_MINUS: SetPrevVelocity(g); break;

        case KEY_KP_ADD:
        case KEY_EQUAL: SetNextVelocity(g); break;

        case KEY_ZERO ... KEY_NINE:
        {
            int i_maze = (key == KEY_ZERO) ? 9 : key - KEY_ONE;
            if (i_maze < g->totalScreens) {
                if (sd->state == SEARCHING)
                    sd->state = NOT_SEARCHING;
                g->currentScreen = i_maze;
            }
            return;
        }
    }

    // Manual controls
    if (sd->type == MANUAL) {
        if (sd->state != FOUND) {
            switch (key) {
                case KEY_UP: p->direction = DIR_UP; break;
                case KEY_DOWN: p->direction = DIR_DOWN; break;
                case KEY_RIGHT: p->direction = DIR_RIGHT; break;
                case KEY_LEFT: p->direction = DIR_LEFT; break;
                default: break;
            }
        }
    }
}

float GetCurrentVelocity(const Game *g)
{
    return VELOCITY_PRESETS[g->currentVelocity];
}

void UpdateTitle(Game *g)
{
    if (GetKeyPressed() != 0) {
        g->currentState = GAMEPLAY_STATE;
    }
}

void UpdateGameplay(Game *g)
{
    HandleKeyInput(g);

    HandleMouseInput(g);
    UpdateButtonStates(g);

    SearchData *sd = GetCurrentSearchData(g);
    Player *p = GetCurrentPlayer(g);

    if (sd->state == SEARCHING && sd->type != MANUAL) {
        SearchStep(sd, GetCurrentVelocity(g));
        p->x = sd->currentNode->x;
        p->y = sd->currentNode->y;
    }
}
