#include "gamelogic.h"
#include "asteroid.h"
#include "autils.h"
#include "structs.h"
#include <raylib.h>

void OnPlayerAccellerate(ObjectStruct *object, float speed) {
    object->speed.x += object->shape.points[0].x * (speed * GetFrameTime());
    object->speed.y += object->shape.points[0].y * (speed * GetFrameTime());
}


#ifdef DEBUGGING
void DebugingKeyHandler(ObjectTracker *tracker) {

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
        LOG(DEBUG, "%s", "CTRL and C is pressed");
        GAME_STATE = EXIT;
    }

    if (IsKeyPressed('L')) {
        FPS_TARGET = FPS_TARGET ? 0 : 75;
        SetTargetFPS(FPS_TARGET);
    }

    if (IsKeyPressed('T')) {
        GDB_BREAK = !GDB_BREAK;
        printf("Breakpoint says hi\n");
    }
    if (IsKeyPressed('V')) {
        VISUAL_DEBUG = !VISUAL_DEBUG;
    }

    if (IsKeyPressed('O')) {
        tracker->objList[0]->objPtr->position = (Vector2){ 0, 0 };
        tracker->objList[0]->objPtr->speed = (Vector2){ 0, 0 };
    }

    if (GAME_STATE == RUNNING) {
        if (IsKeyPressed('P'))
            DEBUG_PAUSE = !DEBUG_PAUSE;
        if (IsKeyPressed('1')) {

            CreateAsteroid(
                tracker, (Vector2){ -200, 0 }, (Vector2){ 0, 0 }, 1, 1);
            /* CreateAsteroid( */
            /*     tracker, (Vector2){ 300, 600 }, (Vector2){ 0, 0 }, 0, 1); */
            /* CreateAsteroid( */
            /*     tracker, (Vector2){ 500, 600 }, (Vector2){ -90, 0 }, 0, 1);
             */

            /* CreateAsteroid( */
            /*     tracker, (Vector2){ 300, 900 }, (Vector2){ 90, 0 }, 0, 1); */
            /* CreateAsteroid( */
            /*     tracker, (Vector2){ 500, 900 }, (Vector2){ 0, 0 }, 0, 1); */

            /* CreateAsteroid( */
            /*     tracker, (Vector2){ 300, 300 }, (Vector2){ 90, 0 }, 0, 1); */
            /* CreateAsteroid( */
            /*     tracker, (Vector2){ 500, 300 }, (Vector2){ -90, 0 }, 0, 1);
             */
        }

        if (IsKeyPressed('2')) {
            CreateAsteroid(
                tracker, (Vector2){ 300, 600 }, (Vector2){ 0, 0 }, 0, 2);
            CreateAsteroid(
                tracker, (Vector2){ 500, 600 }, (Vector2){ -90, 0 }, 0, 1);

            CreateAsteroid(
                tracker, (Vector2){ 300, 900 }, (Vector2){ 90, 0 }, 0, 1);
            CreateAsteroid(
                tracker, (Vector2){ 500, 900 }, (Vector2){ 0, 0 }, 0, 2);
        }

        if (IsKeyPressed('3')) {

            /* CreateAsteroid( */
            /*     tracker, (Vector2){ 300, 600 }, (Vector2){ 90, 0 }, 0, 1); */
            /* CreateAsteroid( */
            /*     tracker, (Vector2){ 500, 600 }, (Vector2){ -90, 0 }, 0, 2);
             */

            CreateAsteroid(
                tracker, (Vector2){ 300, 900 }, (Vector2){ -30, 0 }, 0, 4);
            CreateAsteroid(
                tracker, (Vector2){ 600, 900 }, (Vector2){ -90, 0 }, 0, 1);
        }

        if (IsKeyPressed('4')) {

            CreateAsteroid(
                tracker, (Vector2){ 400, 300 }, (Vector2){ 120, 0 }, 0, 1);
            CreateAsteroid(
                tracker, (Vector2){ 600, 300 }, (Vector2){ -120, 0 }, 0, 2);
        }

        if (IsKeyPressed('5')) {

            CreateAsteroid(
                tracker, (Vector2){ 100, 300 }, (Vector2){ 30, 0 }, 0, 2);

            CreateAsteroid(
                tracker, (Vector2){ 400, 300 }, (Vector2){ 0, 0 }, 0, 1);
            CreateAsteroid(
                tracker, (Vector2){ 900, 300 }, (Vector2){ -120, 0 }, 0, 2);
        }

        if (IsKeyPressed('9')) {
            for (unsigned int i = 0; i < MAX_OBJECT_COUNT - 1; i++) {
                AsteroidSafeSpawn(tracker);
                /* CreateAsteroid( */
                /*     tracker, */
                /*     (Vector2){ GetRandomFloat(0, (float)SCREEN_WIDTH), */
                /*                GetRandomFloat(0, (float)SCREEN_HEIGHT) }, */

                /*     (Vector2){ GetRandomFloat(-100, 100), */
                /*                GetRandomFloat(-100, 100) }, */
                /*     GetRandomFloat(-5, 5), */
                /*     1); */
            }
        }

        if (IsKeyPressed('B') && BENCH_LOG_FILE_PTR) {
            if (!BENCHRUNNING) {
                BENCHRUNNING = true;
                for (unsigned int i = 0; i < MAX_OBJECT_COUNT - 1; i++) {
                    AsteroidSafeSpawn(tracker);
                    /* CreateAsteroid( */
                    /*     tracker, */
                    /*     (Vector2){ GetRandomFloat(0, (float)SCREEN_WIDTH), */
                    /*                GetRandomFloat(0, (float)SCREEN_HEIGHT) },
                     */

                    /*     (Vector2){ GetRandomFloat(-100, 100), */
                    /*                GetRandomFloat(-100, 100) }, */
                    /*     GetRandomFloat(-5, 5), */
                    /*     1); */
                }
            } else {
                BENCHRUNNING = false;
                unsigned long iterations = tracker->objListLen;
                if (iterations > 1) {
                    for (unsigned long i = iterations - 1; i > 0; i--) {
                        if (tracker->objList[i] == tracker->playerPtr)
                            continue;
                        tracker->objList[i]->request = DELETE;
                    }
                }
            }
        }

        if (IsKeyPressed('0')) {
            for (unsigned int i = 0; i < tracker->objListLen; i++) {
                ObjectWrap *current = tracker->objList[i];
                if (!current || current == tracker->playerPtr)
                    continue;
                current->request = DELETE;
            }
        }

        if (IsKeyPressed('='))
            AsteroidSafeSpawn(tracker);
        /* CreateAsteroid( */
        /*     tracker, */
        /*     (Vector2){ GetRandomFloat( */
        /*                    tracker->objList[0]->objPtr->position.x - 500, */
        /*                    tracker->objList[0]->objPtr->position.x + 500), */
        /*                GetRandomFloat( */
        /*                    tracker->objList[0]->objPtr->position.y - 500, */
        /*                    tracker->objList[0]->objPtr->position.y + 500) },
         */
        /*     (Vector2){ GetRandomFloat(-100, 100), */
        /*                GetRandomFloat(-100, 100) }, */
        /*     GetRandomFloat(-5, 5), */
        /*     GetRandomFloat(0.5, 2)); */

        if (IsKeyPressed('-')) {
            if (tracker->objListLen > 1) {
                ObjectWrap *toDelete =
                    tracker->objList[tracker->objListLen - 1];
                if (toDelete == tracker->playerPtr)
                    toDelete = tracker->objList[tracker->objListLen - 2];
                toDelete->request = DELETE;
            }
        }
    }
}
#endif // DEBUGGING

void ShipControlls(ObjectTracker *tracker) {
    if (IsKeyDown('W'))
        OnPlayerAccellerate(tracker->playerPtr->objPtr, PLAYER_MOVE_SPEED);
    if (IsKeyDown('S'))
        OnPlayerAccellerate(tracker->playerPtr->objPtr, -PLAYER_MOVE_SPEED);
    if (IsKeyDown('D'))
        RotateObject(tracker->playerPtr, PLAYER_ROTATION_SPEED);
    if (IsKeyDown('A'))
        RotateObject(tracker->playerPtr, -PLAYER_ROTATION_SPEED);

    if (IsKeyDown(KEY_SPACE)) {
        if (GetTimeMicS() - lastShot > (long)1e6 / RATE_OF_FIRE) {
            lastShot = GetTimeMicS();
            CreateProjectile(tracker, tracker->playerPtr);
        }
    }
}

void MenuControlls(struct menuParent *menu) {
    if (IsKeyPressed('W')) {
        menu->selected =
            RollOverInt((menu->selected - 1), 0, menu->optionListLen - 1);
        LOG(DEBUG, "Menu->selected == %d", menu->selected);
    }

    if (IsKeyPressed('S')) {
        menu->selected =
            RollOverInt((menu->selected + 1), 0, menu->optionListLen - 1);
        LOG(DEBUG, "Menu->selected == %d", menu->selected);
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        SelectCurrent(menu);

    /* if (IsKeyPressed(KEY_ENTER)) */
    /*     SelectCurrent(menu); */
}

void PlayerRuntimeControlls(ObjectTracker *tracker) {

    if (IsKeyPressed('C'))
        CAMERA_FOLLOW = !CAMERA_FOLLOW;

    if (IsKeyPressed(KEY_ESCAPE)) {
        if (GAME_STATE == PAUSE) {
            LOG(DEBUG, "%s", "Resuming the game");
            GAME_STATE = RUNNING;
        } else if (GAME_STATE == RUNNING) {
            LOG(DEBUG, "%s", "Pauseing the game");
            GAME_STATE = PAUSE;
        }
    }

    if (IsMouseButtonDown(1)) {
        if (CAMERA_FOLLOW)
            CAMERA_FOLLOW = false;
        Vector2 mouseDelta = GetMouseDelta();
        tracker->playerCamera.target.x -=
            mouseDelta.x / tracker->playerCamera.zoom;
        tracker->playerCamera.target.y -=
            mouseDelta.y / tracker->playerCamera.zoom;
    }

    if (CAMERA_FOLLOW) {
        tracker->playerCamera.target.x = tracker->playerPtr->objPtr->position.x;
        tracker->playerCamera.target.y = tracker->playerPtr->objPtr->position.y;
    }
    tracker->playerCamera.zoom = ClampFloat(
        tracker->playerCamera.zoom + GetMouseWheelMove() / 10, 0.3f, 3);
}

void NewGame(ObjectTracker *tracker) {
    lastShot = 0;
    CreatePlayer(tracker, (Vector2){ 0, 0 }, 0.5);
}
