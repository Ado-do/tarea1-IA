#include "game.h"

#include <stdio.h>
#include <stdlib.h>

void BadInput(int curr_maze) {
    fprintf(stderr, "* error: bad input in maze %d\n", curr_maze);
    exit(EXIT_FAILURE);
}

Game *ReadAndInitGame(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "* usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE* input_file = fopen(argv[1], "r");
    if (!input_file) {
        fprintf(stderr, "* error: invalid input file\n");
        exit(EXIT_FAILURE);
    }

    Maze *mazes[MAX_MAZES];
    int n_mazes = 0;
    while (n_mazes < MAX_MAZES) {
        int m, n;
        Position start, end;

        if (fscanf(input_file, "%d", &m) != 1) {
            BadInput(n_mazes);
        }

        if (m == 0) break;

        if (fscanf(input_file, "%d %d %d %d %d", &n, &start.x, &start.y, &end.x, &end.y) != 5) {
            BadInput(n_mazes);
        }

        int **layout = (int **) malloc(m * sizeof(int *));
        for (int i = 0; i < m; i++) {
            layout[i] = (int *) malloc(n * sizeof(int));
            for (int j = 0; j < n; j++) {
                if (fscanf(input_file, "%d", &layout[i][j]) != 1) {
                    BadInput(n_mazes);
                }
            }
        }
        mazes[n_mazes] = InitMaze(m, n, start, end, layout);

        n_mazes++;
    }

    return InitGame(mazes, n_mazes);
}

int main(int argc, char *argv[]) {
    Game *game = ReadAndInitGame(argc, argv);
    RunGame(game);
    FreeGame(game);
    return 0;
}
