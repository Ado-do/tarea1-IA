#include "game.h"
#include "raylib.h"

#include <stdlib.h>

static float time_cnt_s = 0.0f;

Labyrinth *InitLabyrinth(int m, int n, Position start, Position end, int **layout) {
    Labyrinth *lab = (Labyrinth *)malloc(sizeof(Labyrinth));
    *lab = (Labyrinth){
        .m = m,
        .n = n,
        .start = start,
        .end = end,
        .layout = layout,
    };
    return lab;
}

Game *InitGame(Labyrinth *labs[MAX_LABS], int n_labs) {
    Game *game = (Game *)malloc(sizeof(Game));
    *game = (Game){
        .labs = (Labyrinth **)malloc(n_labs * sizeof(Labyrinth *)),
        .n_labs = n_labs,
        .curr_lab = 0,
        .is_running = 1,
        .current_screen = TITLE,
    };
    for (int i = 0; i < n_labs; i++) {
        game->labs[i] = labs[i];
    }
    return game;
}

void HandleTitleInput(Game *g) {
    if (GetKeyPressed() != 0) {
        g->current_screen = GAMEPLAY;
    }
}

void HandleGameplayInput(Game *g) {
    switch (GetKeyPressed()) {
        case KEY_Q: g->is_running = 0; break;
    }
}

void HandleInput(Game *g) {
    g->is_running = !WindowShouldClose();
    switch (g->current_screen) {
        case TITLE:     HandleTitleInput(g); break;
        case GAMEPLAY:  HandleGameplayInput(g); break;
    }
}

void UpdateGameplay(Game *g) {

}

void UpdateGame(Game *g) {
    switch (g->current_screen) {
        case TITLE: break;
        case GAMEPLAY: UpdateGameplay(g);
    }
}

void DrawTitleScreen() {
    const char *t[] = { GAME_NAME, "Press any key to continue" };
    const int s[] = { SCREEN_HEIGHT/4, SCREEN_HEIGHT/16 };
    const int w[] = { MeasureText(t[0], s[0]), MeasureText(t[1], s[1]) };
    const Position p[] = {
        {SCREEN_WIDTH/2 - w[0]/2, SCREEN_HEIGHT/2 - s[0]/2},
        {SCREEN_WIDTH/2 - w[1]/2, SCREEN_HEIGHT/2 - s[1]/2 + s[0] + 3},
    };

    const float flash_interval_s = 0.75f;
    int draw_indication = ((int)(time_cnt_s / flash_interval_s) % 2 == 0);

    DrawText(t[0], p[0].x, p[0].y, s[0], DARKGRAY);
    DrawText(t[0], p[0].x+4, p[0].y+4, s[0], WHITE);
    if (draw_indication) DrawText(t[1], p[1].x, p[1].y, s[1], GRAY);
}

void DrawGameplayScreen() {
    const char *t[] = {
        GAME_NAME
    };
    const int s[] = {
        32
    };
    const int w[] = {
        MeasureText(t[0], s[0])
    };
    const Position p[] = {
        {5, 5},
    };
    const Color colors[] = {
        WHITE,
    };
    int n_texts = sizeof(t) / sizeof(t[0]);

    for (int i = 0; i < n_texts; i++) {
        DrawText(t[0], p[0].x, p[0].y, s[0], colors[0]);
    }

    DrawLine(0, s[0]+5, SCREEN_WIDTH, s[0]+5, WHITE);
    DrawLine(w[0]+5, 0, w[0]+5, s[0]+5, WHITE);
}

void DrawGame(Game *g) {
    ClearBackground(BLACK);
    switch (g->current_screen) {
        case TITLE:    DrawTitleScreen(); break;
        case GAMEPLAY: DrawGameplayScreen(); break;
        default: break;
    }
}

void RunGame(Game *g) {
    SetTargetFPS(60);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_NAME);
    while (g->is_running) {
        time_cnt_s += GetFrameTime();

        HandleInput(g);
        UpdateGame(g);

        BeginDrawing();
        DrawGame(g);
        EndDrawing();
    }
    CloseWindow();
}

void FreeLabyrinth(Labyrinth *l) {
    for (int i = 0; i < l->m; i++) {
        free(l->layout[i]);
    }
    free(l->layout);
}

void FreeGame(Game *g) {
    for (int i = 0; i < g->n_labs; i++) {
        FreeLabyrinth(g->labs[i]);
    }
    free(g->labs);
    free(g);
}
