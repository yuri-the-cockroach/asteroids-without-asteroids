#include "main.h"

#include <err.h>
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Local Headers
#include "src/objecthandler.h"
#include "src/syslogic.h"
#include "src/structs.h"
#include "src/gamelogic.h"
#include "src/render.h"

// Retuns prefilled ObjectStruct with default data


int main(void) {
    signal(SIGINT, SigIntHandler);

    int screenWidth = 1600;
    int screenHeight = 900;

    int fpsTarget = 75;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "asteroids without asteroids");

    screenHeight /= 2;
    SetWindowPosition(0, 0);
    SetWindowMinSize(10, 10);
    SetWindowMaxSize(8192, 8192);

    SetTargetFPS(fpsTarget);

    /*  ---------------------<Object Init>--------------------  */

    ObjectTracker tracker = InitTracker();
    CreatePlayer(&tracker, (Vector2){ 200, 200 }, 0.5);
    char debug[128] = "";

    while (!WindowShouldClose()) {
        // Adjust moving speed for current frametime, so you don't get faster as
        // fps goes up

        SCREEN_WIDTH = GetScreenWidth();
        SCREEN_HEIGHT = GetScreenHeight();

        DebugingKeyHandler(&tracker);
        RunActionList(&tracker);

        BeginDrawing();
        {

            ClearBackground(BLACK);
            DrawAllFromDrawList(&tracker);

            sprintf(debug, "Length of the list: %lu", tracker.objListLen);
            DrawText(debug, 20, 20, 18, RAYWHITE);

            DrawText(debug, 20, 40, 18, RAYWHITE);
            DrawFPS(400, 0);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
