#include "game.h"

#include "button.h"
#include "render.h"
#include "search.h"
#include "update.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

static int LoadInput(Maze *mazes, const char *input_path)
{
    FILE *input_file;
    if (TextIsEqual(input_path, "")) {
        input_file = fopen("../inputs/example.txt", "r");
        if (input_file == NULL) {
            input_file = fopen("inputs/example.txt", "r");
        }
        if (input_file == NULL) {
            TraceLog(LOG_FATAL, "LoadInput error: couldn't open default input file (different working path?)");
        }
    } else {
        input_file = fopen(input_path, "r");
        if (input_file == NULL) {
            TraceLog(LOG_FATAL, "LoadInput error: couldn't open input file");
        }
    }

    int n_mazes = 0;
    while (n_mazes < MAX_MAZES) {
        int rows, cols;
        Position start, goal;

        if (fscanf(input_file, "%d", &rows) != 1)
            TraceLog(LOG_FATAL, "LoadInput error: bad input in maze %d", n_mazes);
        if (rows == 0)
            break;

        if (fscanf(input_file, "%d %d %d %d %d", &cols, &start.y, &start.x, &goal.y, &goal.x) != 5)
            TraceLog(LOG_FATAL, "LoadInput error: bad input in maze %d", n_mazes);

        InitMaze(&mazes[n_mazes], rows, cols, start, goal);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (fscanf(input_file, "%d", &mazes[n_mazes].grid[i][j]) != 1)
                    TraceLog(LOG_FATAL, "LoadInput error: bad input in maze %d", n_mazes);
            }
        }

        n_mazes++;
    }
    return n_mazes;
}

MazeScreen *CreateMazeScreen(Maze *m)
{
    MazeScreen *ms = malloc(sizeof(MazeScreen));

    ms->maze = malloc(sizeof(Maze));
    InitMaze(ms->maze, m->rows, m->cols, m->start, m->goal);
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            ms->maze->grid[i][j] = m->grid[i][j];
        }
    }

    ms->searchData = CreateSearchData(ms->maze);

    InitPlayer(&ms->player, m->start.x, m->start.y);

    return ms;
}

void InitGame(Game *g, const char *input_path)
{
    Maze input_mazes[MAX_MAZES];
    int loaded_mazes = LoadInput(input_mazes, input_path);

    *g = (Game){
        .screens = malloc(loaded_mazes * sizeof(MazeScreen *)),
        .totalScreens = loaded_mazes,
        .currentScreen = 0,
        .currentState = TITLE_STATE,
        .isRunning = true,
        .showControls = false,
        .currentVelocity = DEFAULT_VELOCITY,
        .mazeButtons = malloc(loaded_mazes * sizeof(Button)),
    };

    if (!g->screens || !g->mazeButtons)
        TraceLog(LOG_FATAL, "InitGame: memory allocation failed");

    for (int k = 0; k < loaded_mazes; k++) {
        g->screens[k] = CreateMazeScreen(&input_mazes[k]);
        InitButton(&g->mazeButtons[k], TextFormat("%d", (k == 9) ? 0 : k + 1));
        FreeMaze(&input_mazes[k]);

        // Output requested
        SearchData *sd = g->screens[k]->searchData;
        while (!BFSStep(sd)) continue;
        if (sd->state == NOT_FOUND) {
            printf("No hay solución\n");
        } else {
            printf("%d\n", sd->finalPathLength);
        }
        ResetSearchData(sd);
        sd->type = DFS;
    }

    InitButton(&g->searchButtons[0], "DFS");
    InitButton(&g->searchButtons[1], "BFS");
    InitButton(&g->searchButtons[2], "UCS");
    InitButton(&g->searchButtons[3], "A*");
    InitButton(&g->searchButtons[4], "MANUAL");

    InitButton(&g->actionButton, "SOLVE");
    InitButton(&g->addVelButton, "+");
    InitButton(&g->rmVelButton, "-");
}

void DeleteMazeScreen(MazeScreen *ms)
{
    FreeMaze(ms->maze);
    DeleteSearchData(ms->searchData);
    free(ms);
}

void FreeGame(Game *g)
{
    if (g == NULL)
        return;
    for (int i = 0; i < g->totalScreens; i++) {
        DeleteMazeScreen(g->screens[i]);
    }
    free(g->screens);
    g->screens = NULL;

    free(g->mazeButtons);
    g->mazeButtons = NULL;
}

void ChangeCurrentSearchType(Game *g, SearchType searchType)
{
    SearchData *sd = GetCurrentSearchData(g);
    Maze *m = GetCurrentMaze(g);
    ResetSearchData(sd);
    sd->type = searchType;
    InitPlayer(GetCurrentPlayer(g), m->start.x, m->start.y);
}

void UpdateGame(Game *g)
{
    g->isRunning = !WindowShouldClose();
    switch (g->currentState) {
        case TITLE_STATE: UpdateTitle(g); break;
        case GAMEPLAY_STATE: UpdateGameplay(g); break;
    }
}

void DrawGame(Game *g)
{
    BeginDrawing();
    switch (g->currentState) {
        case TITLE_STATE: DrawTitle(g); break;
        case GAMEPLAY_STATE: DrawGameplay(g); break;
    }
    EndDrawing();
}

MazeScreen *GetCurrentScreen(const Game *g)
{
    return g->screens[g->currentScreen];
}

Player *GetCurrentPlayer(const Game *g)
{
    return &GetCurrentScreen(g)->player;
}

SearchData *GetCurrentSearchData(const Game *g)
{
    return GetCurrentScreen(g)->searchData;
}

SearchType CurrentSearchType(const Game *g)
{
    return GetCurrentSearchData(g)->type;
}

Maze *GetCurrentMaze(const Game *g)
{
    return GetCurrentScreen(g)->maze;
}
