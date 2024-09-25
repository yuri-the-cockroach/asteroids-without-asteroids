#include <err.h>
#include <errno.h>
#include <execinfo.h>
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// Local Headers
#include "src/asteroidsutils.h"
#include "src/gamelogic.h"
#include "src/objecthandler.h"
#include "src/render.h"
#include "src/structs.h"
#include "src/syslogic.h"
#include "src/logger.h"

// Retuns prefilled ObjectStruct with default data

int SCREEN_WIDTH = 1600;
int SCREEN_HEIGHT = 900;
int FPS_TARGET = 75;
enum loglevel CURRENT_LOG_LEVEL = DEFAULT_LOG_LEVEL;
bool VISUAL_DEBUG = true;

Camera2D camera = { 0 };
long lastShot = 0;
bool CAMERA_FOLLOW = true;
bool PAUSE_GAME = false;
bool GDB_STOP = false;

int main(int argc, char **argv) {
    errno = 0;



    LOG(ERROR, "%s", "hi");
    LOG(WARNING, "%s", "hi");

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if ((strcmp(argv[i], "-l") || strcmp(argv[i], "--loglevel")) &&
                i < argc - 1)
            {
                i++;
                int newLogLevel = ClampInt(atoi(argv[i]), 0, 8);
                if (newLogLevel) {
                    CURRENT_LOG_LEVEL = (enum loglevel)newLogLevel;
                    continue;
                }

                if (strcmp(argv[i], "0")) {
                    CURRENT_LOG_LEVEL = 0;
                    continue;
                }
            }
        }
    }

    lastShot = 0;
    signal(SIGINT, SigIntHandler);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids without asteroids");

    SetWindowPosition(0, 0);
    SetWindowMinSize(10, 10);
    SetWindowMaxSize(8192, 8192);

    SetTargetFPS(FPS_TARGET);

    /*  ---------------------<Object Init>--------------------  */

    ObjectTracker tracker = InitTracker();
    CreatePlayer(&tracker, (Vector2){ 200, 200 }, 0.5);
    char debug[128] = "";

    camera.target = (Vector2){ tracker.objList[0]->objPtr->position.x + 20.0f,
                               tracker.objList[0]->objPtr->position.y + 20.0f };
    camera.offset =
        (Vector2){ (float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        // Adjust moving speed for current frametime, so you don't get faster as
        // fps goes up

        SCREEN_WIDTH = GetScreenWidth();
        SCREEN_HEIGHT = GetScreenHeight();
        camera.offset.x = (float)SCREEN_WIDTH / 2;
        camera.offset.y = (float)SCREEN_HEIGHT / 2;

        DebugingKeyHandler(&tracker);
        KeyboardHandler(tracker.objList[0]->objPtr);

        RunActionList(&tracker);

        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode2D(camera);
            {
                DrawRectangle(WORLD_POS_MIN_X,
                          WORLD_POS_MIN_Y,
                          abs(WORLD_POS_MIN_X) + abs(WORLD_POS_MAX_X),
                          abs(WORLD_POS_MIN_Y) + abs(WORLD_POS_MAX_Y),
                          (Color){ 18, 18, 18, 255 });
                DrawGrid2D(200, (Color){ 38, 38, 38, 255 });
                DrawAllFromDrawList(&tracker);
            }
            EndMode2D();
            sprintf(debug, "Length of the list: %lu", tracker.objListLen);
            DrawText(debug, 20, 20, 18, RAYWHITE);

            if ( PAUSE_GAME ) DrawText("GAME PAUSED", SCREEN_WIDTH / 2 - 100, 100, 32, WHITE);
            DrawFPS(0, 0);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
