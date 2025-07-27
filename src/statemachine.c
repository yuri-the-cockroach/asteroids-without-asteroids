#include "statemachine.h"
#include "autils.h"
#include "collision.h"
#include "gamelogic.h"
#include "logger.h"
#include "menulogic.h"
#include "mt.h"
#include "objecthandler.h"
#include "objectlogic.h"
#include "render.h"
#include "structs.h"

#ifdef BENCHMARKING
    #include "benchmarking.h"
#endif // BENCHMARKING

#include "raylib.h"
#include <stdio.h>
#include <time.h>

enum game_state GAME_STATE = INIT;
enum game_state NEXT_STATE = NOOP;

// Set default values for basic variables

int StateMachine(void) {
    LOG(DEBUG,
        "<--- CURRENT CONFIG --->\n"
        "WORLD_POS_MIN_X = %d\n"
        "WORLD_POS_MAX_X = %d",
        WORLD_POS_MIN_X,
        WORLD_POS_MAX_X);
#ifdef BENCHMARKING
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
            LOG(ALL, "Entered INIT state");
            GAME_STATE = MAIN_MENU;
            LOG(ALL, "Exiting INIT state");
            break;
        }

        case MAIN_MENU: {
            curMenu = MenuControlls(curMenu, &menuHighlighted);
            RunMenuRender(curMenu, menuHighlighted, 0);
            break;
        }

        case TESTING: {
            tracker    = InitTracker();
            mtDataWrap = InitMT(tracker);
            curMenu    = &refMainMenu;
            GAME_STATE = RUNNING;
            break;
        }

        case START_NEW: {
            tracker    = InitTracker();
            mtDataWrap = InitMT(tracker);
            NewGame(tracker);
            curMenu    = &refMainMenu;
            GAME_STATE = RUNNING;
            break;
        }

        case GAME_OVER: {
            char msg[128] = "";
            sprintf(msg, "Your score: %ud", tracker->playerScore);
            curMenu = MenuControlls(curMenu, &menuHighlighted);
            RunMenuRender(curMenu, menuHighlighted, 1, msg);
            break;
        }

        case RUNNING: {
            LOG(ALL, "Entered RUNNING state");
            DEBUG(if (tracker->playerPtr) SPEED_PREV =
                      tracker->playerPtr->objPtr->speed;
                  DebugingKeyHandler(tracker);)

            // Controlls
            PlayerRuntimeControlls(tracker);
            ShipControlls(tracker);

            SpawnAsteroidOnTime(tracker);
            // RunActionList(tracker);
            SortListByX(tracker);
            DEBUG(if (!DEBUG_PAUSE)) {
                RunThreads(mtDataWrap);
                CollectThreads(mtDataWrap);
                objWrap *current;
                RunActionList(tracker);
                GAME_TIME_PASSED += GetFrameTime();
            }

            // Logic
            // Rendering
            RunWorldRender(tracker);
            RunScreenRender(tracker);

            CleanupMemory(tracker);
            LOG(ALL, "Exiting RUNNING state");
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
        LAST_FRAME_TIME = GetFrameTime();
    }
}
