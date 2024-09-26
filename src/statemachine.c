#include "statemachine.h"
#include "gamelogic.h"
#include "menulogic.h"
#include "objecthandler.h"
#include "structs.h"
#include <raylib.h>

enum game_state GAME_STATE = INIT;

// Set default values for basic variables
void RunConfig(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids without asteroids");

    SetWindowPosition(1, 1);
    SetWindowMinSize(640, 480);
    SetWindowMaxSize(8192, 8192);

    SetTargetFPS(FPS_TARGET);
}

int StateMachine(void) {

    RunConfig();
    ObjectTracker *tracker = NULL;

    struct menuParent mainMenu = SetupMainMenu();
    struct menuParent pauseMenu = SetupPauseMenu();
    /* struct menuStack mStack = (struct menuStack){ */
    /*     0, calloc(MAX_MENU_STACK_SIZE, sizeof(struct menuParent)) */
    /* }; */

    /* MenuStackPush(&mStack, mainMenu); */

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
                RunMenuRender(&mainMenu, "ASTEROIDS WITHOUT ASTEROIDS");
                break;
            }

            case START_NEW: {

                tracker = InitTracker();
                NewGame(tracker);
                GAME_STATE = RUNNING;
                break;
            }

            case GAME_OVER: {
                DeleteTracker(tracker);
                GAME_STATE = MAIN_MENU;
                break;
            }

            case RUNNING: {

                if (DEBUGGING)
                    DebugingKeyHandler(tracker);

                // Controlls
                PlayerRuntimeControlls(tracker);
                ShipControlls(tracker);

                // Logic
                if (!DEBUG_PAUSE)
                    RunActionList(tracker);

                // Rendering
                RunWorldRender(tracker);
                RunScreenRender(tracker);
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

                RunMenuRender(&pauseMenu, "GAME PAUSED");
                break;
            }

            case EXIT: {
                return 0;
            }
        }

        EndDrawing();
    }
}
