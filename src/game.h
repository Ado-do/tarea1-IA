#ifndef GAME_H
#define GAME_H

#define GAME_NAME "JumpLab"
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT (int)(SCREEN_WIDTH * (9.0 / 16.0))
#define MAX_LABS 10

typedef enum { TITLE, GAMEPLAY } GameScreen;

typedef struct {
    int x, y;
} Position;

typedef struct {
    int m, n;
    Position start, end;
    int **layout;
} Labyrinth;

typedef struct {
    Labyrinth **labs;
    int n_labs;
    int curr_lab;
    int is_running;
    GameScreen current_screen;
} Game;


Labyrinth *InitLabyrinth(int, int, Position, Position, int **);
Game *InitGame(Labyrinth *[MAX_LABS], int);

void RunGame(Game *);

void FreeGame(Game *);

#endif // GAME_H
