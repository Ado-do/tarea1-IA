#include "game.h"

#include <stdio.h>
#include <stdlib.h>

Game *ReadAndInitGame() {
    Labyrinth *labs[MAX_LABS];
    int n_labs = 0;

    while (n_labs < MAX_LABS) {
        int m, n;
        Position start, end;

        if (scanf("%d", &m) != 1) {
            fprintf(stderr, "error: bad input in lab %d\n", n_labs);
            exit(EXIT_FAILURE);
        }

        if (m == 0) break;

        if (scanf("%d%d%d%d%d", &n, &start.x, &start.y, &end.x, &end.y) != 5) {
            fprintf(stderr, "error: bad input in lab %d\n", n_labs);
            exit(EXIT_FAILURE);
        }

        int **layout = (int **) malloc(m * sizeof(int *));
        for (int i = 0; i < m; i++) {
            layout[i] = (int *) malloc(n * sizeof(int));
            for (int j = 0; j < n; j++) {
                scanf("%d", &layout[i][j]);
            }
        }
        labs[n_labs] = InitLabyrinth(m, n, start, end, layout);

        n_labs++;
    }

    return InitGame(labs, n_labs);
}

int main() {
    Game *game = ReadAndInitGame();
    RunGame(game);
    FreeGame(game);
    return EXIT_SUCCESS;
}
