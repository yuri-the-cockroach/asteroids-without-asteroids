#include "statemachine.h"
#include "autils.h"
#include "gamelogic.h"
#include "logger.h"
#include "menulogic.h"
#include "objecthandler.h"
#include "render.h"
#include "structs.h"
#ifdef BENCHMARKING
    #include "benchmarking.h"
#endif // BENCHMARKING

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
    #ifdef BENCHMARKING
    long timerStartTotalCycle = 0;
    long timerWorldRender = 0;
    long timerScreenRender = 0;
    // long timerStartKeys = 0;
    #endif // BENCHMARKING

    objTracker *tracker = NULL;

    const menuParent *curMenu = &refMainMenu;
    int menuHighlighted = 0;

    while (true) {

        BeginDrawing();
        ClearBackground(BLACK);

        switch (GAME_STATE) {
            case INIT: {
                GAME_STATE = MAIN_MENU;
                break;
            }

            case MAIN_MENU: {
                curMenu = MenuControlls(curMenu, &menuHighlighted);
                RunMenuRender(curMenu, menuHighlighted, "ASTEROIDS WITHOUT ASTEROIDS", 0);
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
                MenuControlls(curMenu, &menuHighlighted);
                RunMenuRender(curMenu, menuHighlighted, "GAME OVER", 1, msg);
                break;
            }

            case RUNNING: {

                LOG(BENCH, "%s", "<--- Started frame cycle --->");
                #ifdef BENCHMARKING
                    BenchStart(&timerStartTotalCycle);
                #endif // BENCHMARKING

                SPEED_PREV = tracker->playerPtr->objPtr->speed;
                #ifdef DEBUGGING
                    DebugingKeyHandler(tracker);
                    if (!DEBUG_PAUSE) {
                        SpawnAsteroidOnTime(tracker);
                        RunActionList(tracker);
                        GAME_TIME_PASSED += GetFrameTime();
                    }
                #else
                    SpawnAsteroidOnTime(tracker);
                    RunActionList(tracker);
                    GAME_TIME_PASSED += GetFrameTime();
                #endif // DEBUGGING

                // Controlls
                PlayerRuntimeControlls(tracker);
                ShipControlls(tracker);

                // Logic



                // Rendering

                #ifdef BENCHMARKING
                    BenchStart(&timerWorldRender);
                    RunWorldRender(tracker);
                    BenchEnd(&timerWorldRender, "World Renderer");
                    BenchStart(&timerScreenRender);
                    RunScreenRender(tracker);
                    BenchEnd(&timerScreenRender, "Screen Renderer");
                    BenchEnd(&timerStartTotalCycle, "Total cycle");
                    LOG(BENCH, "%s", "<--- Ended frame cycle --->\n");
                #else
                    RunWorldRender(tracker);
                    RunScreenRender(tracker);
                #endif // BENCHMARKING

                break;
            }

            case PAUSE: {

                #ifdef DEBUGGING
                    DebugingKeyHandler(tracker);
                #endif // DEBUGGING

                // Controlls
                curMenu = &refPauseMenu;
                PlayerRuntimeControlls(tracker);
                MenuControlls(curMenu, &menuHighlighted);

                // Rendering
                RunWorldRender(tracker);
                RunScreenRender(tracker);

                RunMenuRender(curMenu, menuHighlighted, "GAME PAUSED", 0);
                break;
            }

            case EXIT: {
                CloseWindow();
                return 0;
            }

            case CLEANUP: {
                curMenu = &refMainMenu;
                DeleteTracker(tracker);
                tracker = 0;
                if ( NEXT_STATE == NOOP ) GAME_STATE = EXIT;
                GAME_TIME_PASSED = 0;
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
