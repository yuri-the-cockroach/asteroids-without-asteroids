#include <err.h>
#include <errno.h>
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <execinfo.h>

// Local Headers
#include "src/logger.h"
#include "src/structs.h"
#include "src/objecthandler.h"
#include "src/syslogic.h"
#include "src/gamelogic.h"
#include "src/render.h"
#include "src/utils.h"

// Retuns prefilled ObjectStruct with default data

int SCREEN_WIDTH = 1600;
int SCREEN_HEIGHT = 900;
int FPS_TARGET = 75;
enum loglevel CURRENT_LOG_LEVEL = DEFAULT_LOG_LEVEL;

long lastShot = 0;


int main(int argc, char** argv) {
    errno = 0;

    if ( argc > 1 ) {
        for ( int i = 1; i < argc; i++ ) {
            if ((strcmp(argv[i], "-l") || strcmp(argv[i], "--loglevel")) && i < argc - 1) {
                i++;
                int newLogLevel = clampInt(atoi(argv[i]), 0, 8);
                if (newLogLevel) {
                    CURRENT_LOG_LEVEL = (enum loglevel)newLogLevel;
                    continue;
                }

                if ( strcmp(argv[i], "0") ) {
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

            DrawFPS(400, 0);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
