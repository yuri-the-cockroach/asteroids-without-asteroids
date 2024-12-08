#include "statemachine.h"
#include "asteroidsutils.h"
#include "gamelogic.h"
#include "logger.h"
#include "menulogic.h"
#include "objecthandler.h"
#include "render.h"
#include "structs.h"
#include "benchmarking.h"

#include <raylib.h>
#include <stdio.h>
#include <time.h>

enum game_state GAME_STATE = INIT;
enum game_state NEXT_STATE = NOOP;

// Set default values for basic variables
void RunConfig(void) {

    #ifdef BENCHMARKING
    BENCH_LOG_FILE_PTR = fopen(BENCH_LOG_FILE_NAME, "w");
    LOG(DEBUG, "%s", "Compiled with benchmarking support.\nBenchmarking is enabled");

    #endif

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids without asteroids");

    SetWindowPosition(1, 1);
    SetWindowMinSize(640, 480);
    SetWindowMaxSize(8192, 8192);

    SetTargetFPS(FPS_TARGET);
    CreateLogFile();
}

int StateMachine(void) {
    long timerStartTotalCycle = 0;
    long timerWorldRender = 0;
    long timerScreenRender = 0;
    long timerStartKeys = 0;

    ObjectTracker *tracker = NULL;

    struct menuParent mainMenu = SetupMainMenu();
    struct menuParent pauseMenu = SetupPauseMenu();
    struct menuParent gameOverMenu = SetupGameOverMenu();

    while (true) {

        BeginDrawing();
        ClearBackground(BLACK);

        switch (GAME_STATE) {
            case INIT: {
                GAME_STATE = MAIN_MENU;
                break;
            }

            case MAIN_MENU: {
                MenuControlls(&mainMenu);
                RunMenuRender(&mainMenu, "ASTEROIDS WITHOUT ASTEROIDS", 0);
                break;
            }

            case START_NEW: {

                tracker = InitTracker();
                NewGame(tracker);
                GAME_STATE = RUNNING;
                break;
            }

            case GAME_OVER: {
                char msg[128] = "";
                sprintf(msg, "Your score: %d", tracker->playerScore);
                MenuControlls(&gameOverMenu);
                RunMenuRender(&gameOverMenu, "GAME OVER", 1, msg);
                break;
            }

            case RUNNING: {

                LOG(BENCH, "%s", "<--- Started frame cycle --->");
                BenchStart(&timerStartTotalCycle);
                if (DEBUGGING)
                    DebugingKeyHandler(tracker);

                // Controlls
                PlayerRuntimeControlls(tracker);
                ShipControlls(tracker);

                // Logic
                if (!DEBUG_PAUSE)
                    RunActionList(tracker);


                // Rendering

                BenchStart(&timerWorldRender);
                RunWorldRender(tracker);
                BenchEnd(&timerWorldRender, "World Renderer");

                BenchStart(&timerScreenRender);
                RunScreenRender(tracker);
                BenchEnd(&timerScreenRender, "Screen Renderer");

                BenchEnd(&timerStartTotalCycle, "Total cycle");
                LOG(BENCH, "%s", "<--- Ended frame cycle --->\n");
                break;
            }

            case PAUSE: {

                if (DEBUGGING)
                    DebugingKeyHandler(tracker);
                // Controlls
                PlayerRuntimeControlls(tracker);
                MenuControlls(&pauseMenu);

                // Rendering
                RunWorldRender(tracker);
                RunScreenRender(tracker);

                RunMenuRender(&pauseMenu, "GAME PAUSED", 0);
                break;
            }

            case EXIT: {
                return 0;
            }

            case CLEANUP: {
                DeleteTracker(tracker);
                if ( NEXT_STATE == NOOP ) GAME_STATE = EXIT;
                GAME_STATE = NEXT_STATE;
                NEXT_STATE = NOOP;
                break;
            }

            case NOOP: {
                err(-1, "Game is stuck in noop! This state is not supposed to be used");
            }
        }

        EndDrawing();
    }
}
