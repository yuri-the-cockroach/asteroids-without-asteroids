#include "raylib.h"
#include <string.h>

#include "asteroid.h"
#include "autils.h"
#include "collision.h"
#include "gamelogic.h"
#include "menulogic.h"
#include "structs.h"
#include "visdebugger.h"

void OnPlayerAccellerate(object *object, float speed) {
    float mult_x = 1;
    float mult_y = 1;

    if (fabsf(object->speed.x + speed) > fabsf(object->speed.x)) {
        mult_x = fabsf(object->speed.x / 500.f);
        mult_x = mult_x > 1 ? mult_x : 1;
    }

    if (fabsf(object->speed.y + speed) > fabsf(object->speed.y)) {
        mult_y = fabsf(object->speed.y / 500.f);
        mult_y = mult_y > 1 ? mult_y : 1;
    }

    object->speed.x +=
        (speed / mult_x) * LAST_FRAME_TIME * object->shape.points[0].x;
    object->speed.y +=
        (speed / mult_y) * LAST_FRAME_TIME * object->shape.points[0].y;

    // object->speed.x +=  object->shape.points[0].x * (speed *
    // LAST_FRAME_TIME); object->speed.y += object->shape.points[0].y * (speed *
    // LAST_FRAME_TIME);
}

#ifdef DEBUGGING
void DebugingKeyHandler(objTracker *tracker) {

    // Allows to drag asteroids
    Vector2 cursorPos =
        (Vector2){ (GetMousePosition().x - tracker->playerCamera.offset.x) /
                           tracker->playerCamera.zoom +
                       tracker->playerCamera.target.x,
                   (GetMousePosition().y - tracker->playerCamera.offset.y) /
                           tracker->playerCamera.zoom +
                       tracker->playerCamera.target.y };

    objWrap *some = NULL;
    if (IsMouseButtonDown(0) &&
        ((some = FindCollisionPos(tracker, cursorPos)) || lastDragged)) {
        if (some) { lastDragged = some; }
        some = !some && lastDragged ? lastDragged : some;

        some->objPtr->position.x = cursorPos.x;
        some->objPtr->position.y = cursorPos.y;
    }

    if (lastDragged && !IsMouseButtonDown(0)) {
        lastDragged->objPtr->speed =
            (Vector2){ GetMouseDelta().x / LAST_FRAME_TIME,
                       GetMouseDelta().y / LAST_FRAME_TIME };

        lastDragged = NULL;
    } // End of dragging

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
    if (IsKeyPressed('V')) { VISUAL_DEBUG = !VISUAL_DEBUG; }

    if (IsKeyPressed('O')) {
        tracker->objList[0]->objPtr->position = (Vector2){ 0, 0 };
        tracker->objList[0]->objPtr->speed    = (Vector2){ 0, 0 };
    }

    if (GAME_STATE == RUNNING) {
        if (IsKeyPressed('P')) DEBUG_PAUSE = !DEBUG_PAUSE;
        if (IsKeyPressed('1')) {
            CreateAsteroid(
                tracker, (Vector2){ 300, 600 }, (Vector2){ 0, 0 }, 0, 1);
            CreateAsteroid(
                tracker, (Vector2){ 500, 600 }, (Vector2){ -90, 0 }, 0, 1);

            CreateAsteroid(
                tracker, (Vector2){ 300, 900 }, (Vector2){ 90, 0 }, 0, 1);
            CreateAsteroid(
                tracker, (Vector2){ 500, 900 }, (Vector2){ 0, 0 }, 0, 1);
        }

        if (IsKeyPressed('2')) {
            CreateAsteroid(
                tracker, (Vector2){ 300, 600 }, (Vector2){ 0, 0 }, 0, 1);
            CreateAsteroid(
                tracker, (Vector2){ 500, 600 }, (Vector2){ -90, 0 }, 0, 2);

            CreateAsteroid(
                tracker, (Vector2){ 300, 900 }, (Vector2){ 90, 0 }, 0, 1);
            CreateAsteroid(
                tracker, (Vector2){ 500, 900 }, (Vector2){ 0, 0 }, 0, 2);
        }

        if (IsKeyPressed('3')) {
            CreateAsteroid(
                tracker, (Vector2){ 600, 300 }, (Vector2){ 0, 0 }, 0, 2);
            CreateAsteroid(
                tracker, (Vector2){ 600, 500 }, (Vector2){ 0, -90 }, 0, 1);
            CreateAsteroid(
                tracker, (Vector2){ 900, 300 }, (Vector2){ 0, 90 }, 0, 1);
            CreateAsteroid(
                tracker, (Vector2){ 900, 500 }, (Vector2){ 0, 0 }, 0, 2);
        }

        if (IsKeyPressed('4')) {

            CreateAsteroid(
                tracker, (Vector2){ 300, 300 }, (Vector2){ 120, 120 }, 0, 1);
            CreateAsteroid(
                tracker, (Vector2){ 600, 600 }, (Vector2){ -120, -120 }, 0, 2);
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
            for (unsigned int i = 0; i < SOFT_MAX_ASTEROIDS - 1; i++) {
                AsteroidSafeSpawn(tracker);
            }
        }

    #ifdef BENCHMARKING
        if (IsKeyPressed('B') && BENCH_LOG_FILE_PTR) {
            if (!BENCHRUNNING) {
                BENCHRUNNING = true;
                for (unsigned int i = 0; i < MAX_OBJECT_COUNT - 1; i++) {
                    AsteroidSafeSpawn(tracker);
                }
            } else {
                BENCHRUNNING             = false;
                unsigned long iterations = tracker->objListLen;
                if (iterations > 1) {
                    for (unsigned long i = iterations - 1; i > 0; i--) {
                        if (tracker->objList[i] == tracker->playerPtr) continue;
                        tracker->objList[i]->request = DELETE;
                    }
                }
            }
        }
    #endif // BENCHMARKING

        if (IsKeyPressed('0')) {
            for (unsigned int i = 0; i < tracker->objListLen; i++) {
                objWrap *current = tracker->objList[i];
                if (!current || current == tracker->playerPtr) continue;
                current->request = DELETE;
            }
        }

        if (IsKeyPressed('=')) AsteroidSafeSpawn(tracker);

        if (IsKeyPressed('-')) {
            if (tracker->objListLen > 1) {
                objWrap *toDelete = tracker->objList[tracker->objListLen - 1];
                if (toDelete == tracker->playerPtr)
                    toDelete = tracker->objList[tracker->objListLen - 2];
                toDelete->request = DELETE;
            }
        }
    }
}
#endif // DEBUGGING

void ShipControlls(objTracker *tracker) {

    if (!tracker->playerPtr) return;
    if (IsKeyDown('W'))
        OnPlayerAccellerate(tracker->playerPtr->objPtr, PLAYER_MOVE_SPEED);
    if (IsKeyDown('S'))
        OnPlayerAccellerate(tracker->playerPtr->objPtr, -PLAYER_MOVE_SPEED);
    if (IsKeyDown('D')) RotateObject(tracker->playerPtr, PLAYER_ROTATION_SPEED);
    if (IsKeyDown('A'))
        RotateObject(tracker->playerPtr, -PLAYER_ROTATION_SPEED);

    if (IsKeyDown(KEY_SPACE)) {
        if (GetTimeMicS() - LAST_SHOT > (long)1e6 / RATE_OF_FIRE) {
            LAST_SHOT = GetTimeMicS();
            CreateProjectile(tracker, tracker->playerPtr);
        }
    }
}

const menuParent *MenuControlls(const menuParent *menu, int *menuHighlighted) {
    if (IsKeyPressed('W')) {
        *menuHighlighted =
            RollOver((*menuHighlighted - 1), 0, menu->optionListLen - 1);
        LOG(DEBUG, "*menuHighlighted == %d", *menuHighlighted);
    }

    if (IsKeyPressed('S')) {
        *menuHighlighted =
            RollOver((*menuHighlighted + 1), 0, menu->optionListLen - 1);
        LOG(DEBUG, "*menuHighlighted == %d", *menuHighlighted);
    }

    // strcmp result is inverted, because reasons I guess...
    if (IsKeyPressed(KEY_ESCAPE) &&
        !strcmp(menu->name, refDifficultyMenu.name)) {
        *menuHighlighted = 0;
        return &refMainMenu;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        const int selected = *menuHighlighted;
        *menuHighlighted   = 0;
        return SelectCurrent(menu, selected);
    }

    return menu;
}

void PlayerRuntimeControlls(objTracker *tracker) {

    if (tracker->playerPtr && IsKeyPressed('C')) CAMERA_FOLLOW = !CAMERA_FOLLOW;

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
        if (CAMERA_FOLLOW) CAMERA_FOLLOW = false;
        Vector2 mouseDelta = GetMouseDelta();
        tracker->playerCamera.target.x -=
            mouseDelta.x / tracker->playerCamera.zoom;
        tracker->playerCamera.target.y -=
            mouseDelta.y / tracker->playerCamera.zoom;
    }

    if (tracker->playerPtr && CAMERA_FOLLOW) {
        tracker->playerCamera.target.x = tracker->playerPtr->objPtr->position.x;
        tracker->playerCamera.target.y = tracker->playerPtr->objPtr->position.y;
    }
    tracker->playerCamera.zoom = ClampFloat(
        tracker->playerCamera.zoom + GetMouseWheelMove() / 10, 0.3f, 3);
}

void NewGame(objTracker *tracker) {
    LAST_SHOT = 0;
    CreatePlayer(tracker,
                 (Vector2){ WORLD_POS_MAX_X / 2.f, WORLD_POS_MAX_Y / 2.f },
                 0.5);
}

int SpawnAsteroidOnTime(objTracker *tracker) {
    if (NEXT_ASTEROID_SPAWN >= GAME_TIME_PASSED) return 0;
    LAST_ASTEROID_SPAWN = GAME_TIME_PASSED;
    NEXT_ASTEROID_SPAWN =
        LAST_ASTEROID_SPAWN +
        ((2.f / (logf((float)tracker->playerScore / 100.f + 1.2f))) - 1);

    if (!AsteroidSafeSpawn(tracker)) return -1;
    LOG(TRACE,
        "Spawned a new asteroid. Currently %d spawned already",
        tracker->objListLen);
    return 1;
}
