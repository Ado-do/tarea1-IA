#ifndef GAME_H
#define GAME_H

#define GAME_NAME "JumpingMaze"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define MAX_MAZES 10
#define MAX_COL 100
#define MAX_ROW 100

typedef enum { TITLE, GAMEPLAY } GameScreen;

typedef struct {
    int x, y;
} Position;

typedef struct {
    int m, n;
    Position start, end;
    int **layout;
} Maze;

typedef struct {
    Maze **mazes;
    int n_mazes;
    int curr_maze;
    int is_running;
    GameScreen current_screen;
} Game;


Maze *InitMaze(int, int, Position, Position, int **);
Game *InitGame(Maze *[MAX_MAZES], int);

void RunGame(Game *);

void FreeGame(Game *);

#endif // GAME_H
