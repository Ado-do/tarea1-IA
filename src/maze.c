#include "maze.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void InitMaze(Maze *m, int rows, int cols, Position start, Position end)
{
    *m = (Maze){
        .rows = rows,
        .cols = cols,
        .start = start,
        .goal = end,
        .grid = malloc(rows * sizeof(int *)),
    };

    if (m->grid == NULL) {
        perror("InitMaze error");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++) {
        m->grid[i] = calloc(cols, sizeof(int));
        if (m->grid[i] == NULL) {
            TraceLog(LOG_FATAL, "InitMaze: could'nt allocate m->grid[%d]", i);
        }
    }
}

void FreeMaze(Maze *m)
{
    if (m == NULL)
        return;

    for (int i = 0; i < m->rows; i++) {
        free(m->grid[i]);
    }
    free(m->grid);
    m->grid = NULL;

    m->rows = 0;
    m->cols = 0;
}

int GetJumpValue(const Maze *m, int x, int y)
{
    return m->grid[y][x];
}

bool IsGoalPosition(const Maze *m, int x, int y)
{
    return (x == m->goal.x && y == m->goal.y);
}

bool IsValidPosition(const Maze *m, int x, int y)
{
    return ((x >= 0 && x < m->cols) && (y >= 0 && y < m->rows));
}
