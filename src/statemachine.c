#include "statemachine.h"
#include "autils.h"
#include "collision.h"
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

int StateMachine(void) {
#ifdef BENCHMARKING
    long timerStartTotalCycle = 0;
    // long timerStartKeys = 0;
#endif // BENCHMARKING

    objTracker *tracker = NULL;

    const menuParent *curMenu       = &refMainMenu;
    int menuHighlighted             = 0;
    struct mt_data_wrap *mtDataWrap = NULL;

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
            RunMenuRender(curMenu, menuHighlighted, 0);
            break;
        }

        case TESTING: {
            tracker    = InitTracker();
            GAME_STATE = RUNNING;
            break;
        }
        case START_NEW: {
            tracker    = InitTracker();
            mtDataWrap = InitMT(tracker);
            NewGame(tracker);
            GAME_STATE = RUNNING;
            break;
        }

        case GAME_OVER: {
            char msg[128] = "";
            sprintf(msg, "Your score: %d", tracker->playerScore);
            MenuControlls(curMenu, &menuHighlighted);
            RunMenuRender(curMenu, menuHighlighted, 1, msg);
            break;
        }

        case RUNNING: {

            DEBUG(SPEED_PREV = tracker->playerPtr->objPtr->speed;
                  DebugingKeyHandler(tracker);)

            DEBUG(if (!DEBUG_PAUSE) {)

                CollectThreads(mtDataWrap);
                SpawnAsteroidOnTime(tracker);
                RunActionList(tracker);
                RunThreads(mtDataWrap);
                GAME_TIME_PASSED += GetFrameTime();

                DEBUG(
            })
            DEBUG(if (tracker->playerPtr) SPEED_PREV =
                      tracker->playerPtr->objPtr->speed;
                  DebugingKeyHandler(tracker);
                  if (!DEBUG_PAUSE) {)
                        if (tracker->playerPtr) SpawnAsteroidOnTime(tracker);
                      RunActionList(tracker);
                      GAME_TIME_PASSED += GetFrameTime();
                    DEBUG(
                  })

            // Controlls
            PlayerRuntimeControlls(tracker);
            ShipControlls(tracker);

            // Logic
            // Rendering
            RunWorldRender(tracker);
            RunScreenRender(tracker);

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

            RunMenuRender(curMenu, menuHighlighted, 0);
            break;
        }

        case EXIT: {
            CloseWindow();
            return 0;
        }
        case CLEANUP: {
            curMenu = &refMainMenu;
            DeleteTracker(tracker);
            MTCleanupAndFree(mtDataWrap);
            mtDataWrap = 0;
            tracker    = 0;
            if (NEXT_STATE == NOOP) GAME_STATE = EXIT;
            GAME_TIME_PASSED = 0;
            GAME_STATE       = NEXT_STATE;
            NEXT_STATE       = NOOP;
            break;
        }

        case NOOP: {
            err(-1,
                "Game is stuck in noop! This state is not supposed to be used");
        }
        }

        EndDrawing();
    }
}
