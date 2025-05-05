#pragma once

#include "maze.h"
#include "button.h"
#include "search.h"

#define GAME_NAME "JumpingMaze"
#define MAX_MAZES 10

typedef enum { TITLE_STATE, GAMEPLAY_STATE } GameState;

typedef struct {
    Maze *maze;
    SearchData *searchData;
    Player player;
} MazeScreen;

typedef struct {
    MazeScreen **screens;
    int totalScreens;
    int currentScreen;
    GameState currentState;
    int currentVelocity;
    bool isRunning;
    bool showControls;
    Button *mazeButtons;
    Button searchButtons[5];
    Button addVelButton;
    Button rmVelButton;
    Button actionButton;
} Game;

MazeScreen *CreateMazeScreen(Maze *m);
void DeleteMazeScreen(MazeScreen *ms);
void ChangeCurrentSearchType(Game *g, SearchType searchType);

Player *GetCurrentPlayer(const Game *g);
SearchData *GetCurrentSearchData(const Game *g);
SearchType CurrentSearchType(const Game *g);
Maze *GetCurrentMaze(const Game *g);

void InitGame(Game *g, const char *inputPath);
void FreeGame(Game *g);
void UpdateGame(Game *g);
void DrawGame(Game *g);
