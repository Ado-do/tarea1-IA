#include "game.h"

#include <raylib.h>

const char *GetInputPath(int argc, char *argv[])
{
    if (argc == 2) {
        return argv[1];
    } else if (argc > 2) {
        TraceLog(LOG_FATAL, "Usage: %s <input_file>\n", argv[0]);
    }

    // if not args given, try to open default input file (inputs/example.txt)
    return "";
}

int main(int argc, char *argv[])
{
    Game game;
    InitGame(&game, GetInputPath(argc, argv));

#ifdef DEBUG
    SetTraceLogLevel(LOG_ALL);
#else
    SetTraceLogLevel(LOG_WARNING);
#endif

    const int windowWidth = 1280;
    const int windowHeight = 720;

    SetTargetFPS(60);
    InitWindow(windowWidth, windowHeight, GAME_NAME);

    while (game.isRunning) {
        UpdateGame(&game);
        DrawGame(&game);
    }

    CloseWindow();
    FreeGame(&game);

    return 0;
}
