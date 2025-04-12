#include "game.h"
#include "raylib.h"

#include <stdlib.h>

int init_game(Game *game, int n_labs, int (*specs)[6], int ***layouts) {
    // init labs
    game->n_labs = n_labs;
    game->labs = (Labyrinth *)malloc(n_labs * sizeof(Labyrinth));
    for (int k = 0; k < n_labs; k++) {
        Labyrinth *lab = &game->labs[k];

        // copy spec
        int *spec = specs[k];
        *lab = (Labyrinth){
            .m = spec[0],
            .n = spec[1],
            .start = {spec[2], spec[3]},
            .end = {spec[4], spec[5]},
        };

        // copy layout
        int **layout = layouts[k];
        lab->layout = (int **) malloc(lab->m * sizeof(int *));
        for (int i = 0; i < lab->m; i++) {
            lab->layout[i] = (int *) malloc(lab->n * sizeof(int));
            for (int j = 0; j < lab->n; j++) {
                lab->layout[i][j] = layout[i][j];
            }
        }
    }
    game->curr_lab = 0;

    // init others fields
    game->is_running = 1;
    game->current_screen = TITLE;

    return EXIT_SUCCESS;
}

void draw_game(Game *game) {
    switch (game->current_screen) {
        case TITLE: {
            char *t = "JUMPLAB";
            int ts = 20;
            int tw = MeasureText(t, ts);
            DrawText(t, GetScreenWidth()/2 - tw/2, GetScreenHeight()/2 - ts*2, ts, WHITE);
        } break;
        case GAMEPLAY: {

        } break;
        default: break;
    }
}

void free_game(Game *game) {
    for (int k = 0; k < game->n_labs; k++) {
        for (int i = 0; i < game->labs[k].m; i++) {
            free(game->labs[k].layout[i]);
        }
        free(game->labs[k].layout);
    }
}
