#pragma once

#include <stdbool.h>

typedef struct {
    int x, y;
} Position;

typedef struct {
    int rows, cols;
    Position start, goal;
    int **grid;
} Maze;

void InitMaze(Maze *m, int rows, int cols, Position start, Position goal);
void FreeMaze(Maze *maze);

int GetJumpValue(const Maze *m, int x, int y);
bool IsGoalPosition(const Maze *m, int x, int y);
bool IsValidPosition(const Maze *maze, int x, int y);
