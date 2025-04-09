#include "raylib.h"
#include "game.h"

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
    SetTargetFPS(60);
    InitWindow(SCREEN_W, SCREEN_H, "rayplat");

    bool gameRunning = true;
    while (gameRunning) {
        if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
            gameRunning = false;
        }

        // HandleInput(player);

        // Update(player, plats, n_plats);

        BeginDrawing();
        ClearBackground(BLACK);
        /* Draw game */
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
