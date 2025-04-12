#ifndef GAME_H
#define GAME_H

typedef enum { TITLE, GAMEPLAY } GameScreen;

typedef struct {
    int i, j;
} Position;

typedef struct {
    int m, n;
    Position start, end;
    int **layout;
} Labyrinth;

typedef struct {
    Labyrinth *labs;
    int n_labs;
    int curr_lab;
    int is_running;
    GameScreen current_screen;
} Game;


int init_game(Game *, int, int (*)[6], int ***);

void update_game(Game *);
void draw_game(Game *);

void free_game(Game *);

#endif // GAME_H
