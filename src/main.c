#include "game.h"
#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>

int read_and_init_game(Game*, int);

int main() {
    const int screen_height = 450;
    const int screen_width = 800;
    const int max_labs = 10;

    // SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTargetFPS(60);
    InitWindow(screen_width, screen_height, "jumplab");

    Game game;
    if (read_and_init_game(&game, max_labs)) {
        fprintf(stderr, "error: bad input\n");
        return EXIT_FAILURE;
    }

    bool gameRunning = true;
    while (gameRunning) {
        if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
            gameRunning = false;
        }

        // update_game(&game);

        BeginDrawing();
        ClearBackground(BLACK);

        draw_game(&game);

        EndDrawing();
    }

    CloseWindow();

    free_game(&game);

    return EXIT_SUCCESS;
}

int read_and_init_game(Game *game, int max_labs) {
    int specs[max_labs][6];  // first line of input of each lab (m, n, i_start, j_start, i_end, j_end)
    int **layouts[max_labs]; // layout matrix of each lab
    int n_labs = 0;          // number of labs readed

    //  read input
    int bad_input = 0;
    if (scanf("%d", &specs[n_labs][0]) != 1) {
        return EXIT_FAILURE;
    }

    while (!bad_input && (n_labs < max_labs) && (specs[n_labs][0] != 0)) {
        int *spec = specs[n_labs];
        int *m = &specs[n_labs][0];
        int *n = &spec[1];

        // read line of current labyrinth specs (n, i_start, j_start, i_end, j_end)
        if (scanf("%d%d%d%d%d", n, &spec[2], &spec[3], &spec[4], &spec[5]) != 5) {
            bad_input = 1;
            break;
        }

        // read matrix of labyrinth layout
        layouts[n_labs] = (int **) malloc(*m * sizeof(int *));
        for (int i = 0; i < *m; i++) {
            layouts[n_labs][i] = (int *) malloc(*n * sizeof(int));
        }

        int **layout = layouts[n_labs];
        for (int i = 0; i < *m; i++) {
            for (int j = 0; j < *n; j++) {
                if (scanf("%d", &layout[i][j]) != 1) {
                    bad_input = 1;
                }
            }
            if (bad_input) break;
        }

        // read next lab
        n_labs++;
        scanf("%d", &specs[n_labs][0]);
    }

    if (!bad_input) {
        init_game(game, n_labs, specs, layouts);
        printf("\t* FUNCIONO EL INPUT QLO WEBIAOOO!!!!!\n");
    }

    for (int k = 0; k < n_labs; k++) {
        int *m = &specs[k][0];
        for (int i = 0; i < *m; i++) {
            free(layouts[k][i]);
        }
        free(layouts[k]);
    }

    return (bad_input)? EXIT_FAILURE : EXIT_SUCCESS;
}
