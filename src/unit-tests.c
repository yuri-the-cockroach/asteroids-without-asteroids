#include "raylib.h"

#include "asteroid.h"
#include "autils.h"
#include "collision.h"
#include "gamelogic.h"
#include "mt.h"
#include "objecthandler.h"
#include "render.h"

#pragma GCC diagnostic ignored "-Wfloat-equal"

int TestSort() {

    objTracker *tracker = InitTracker();

    for (unsigned int i = 0; i < SOFT_MAX_ASTEROIDS - 1; i++) {
        AsteroidSafeSpawn(tracker);
    }
    CleanupMemory(tracker);
    SortListByX(tracker);
    objWrap *prev, *current;
    for (unsigned long i = 1; i < tracker->objListLen; i++) {
        prev    = tracker->objList[i - 1];
        current = tracker->objList[i];
        ASSERT(prev->objPtr->position.x <= current->objPtr->position.x);
    }
    DeleteTracker(tracker);
    return 0;
}

int TestCollisionAnother(int iter, int iter_count) {
    objTracker *tracker = InitTracker();
    GAME_TIME_PASSED    = 0;
    objWrap *asteroids[4];
    DEBUG(VISUAL_DEBUG = true;)
    const float duration = 1;

    struct mt_data_wrap *mtDataWrap = InitMT(tracker);

    tracker->playerCamera.zoom   = .5f;
    tracker->playerCamera.target = (Vector2){ 400, 700 };
    asteroids[0]                 = CreateAsteroid(
        tracker, (Vector2){ 300, 300 }, (Vector2){ 120, 120 }, 0, 1);
    asteroids[1] = CreateAsteroid(
        tracker, (Vector2){ 550, 550 }, (Vector2){ -120, -120 }, 0, 2);

    while (GAME_TIME_PASSED < duration && !WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(BLACK);

        CleanupMemory(tracker);
        SortListByX(tracker);

        DisplayText((Vector2){ 700, 20 },
                    24,
                    WHITE,
                    "Time passed: %.2f\n"
                    "Target time: %.2f\n"
                    "Iteration %d/%d\n"
                    "List size: %ld",
                    GAME_TIME_PASSED,
                    duration,
                    iter + 1,
                    iter_count,
                    tracker->objListLen);

        // RunActionList(tracker);

        RunThreads(mtDataWrap);
        CollectThreads(mtDataWrap);
        for (unsigned long i = 0; i < tracker->objListLen; i++) {
            UpdateObjectPos(tracker->objList[i]);
        }
        RunWorldRender(tracker);

        RunScreenRender(tracker);
        PlayerRuntimeControlls(tracker);

        EndDrawing();
        LAST_FRAME_TIME = GetFrameTime();
        GAME_TIME_PASSED += LAST_FRAME_TIME;
    }

    ASSERT(fCutOff(asteroids[0]->objPtr->speed.x, 0) == -120);
    ASSERT(fCutOff(asteroids[0]->objPtr->speed.y, 0) == -120);

    DeleteTracker(tracker);
    MTCleanupAndFree(mtDataWrap);

    return 0;
}

int TestCollisionHorizontal(int iter, int iter_count) {
    objTracker *tracker = InitTracker();
    GAME_TIME_PASSED    = 0;
    objWrap *asteroids[4];
    DEBUG(VISUAL_DEBUG = true;)
    const float duration = 1;
    float small_speed    = 90;

    struct mt_data_wrap *mtDataWrap = InitMT(tracker);

    tracker->playerCamera.zoom   = .5f;
    tracker->playerCamera.target = (Vector2){ 400, 700 };
    asteroids[0] =
        CreateAsteroid(tracker, (Vector2){ 300, 600 }, (Vector2){ 0, 0 }, 0, 2);
    asteroids[1] = CreateAsteroid(
        tracker, (Vector2){ 500, 600 }, (Vector2){ -small_speed, 0 }, 0, 1);
    asteroids[2] =
        CreateAsteroid(tracker, (Vector2){ 500, 900 }, (Vector2){ 0, 0 }, 0, 2);
    asteroids[3] = CreateAsteroid(
        tracker, (Vector2){ 300, 900 }, (Vector2){ small_speed, 0 }, 0, 1);

    while (GAME_TIME_PASSED < duration && !WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(BLACK);

        CleanupMemory(tracker);
        SortListByX(tracker);

        DisplayText((Vector2){ 700, 20 },
                    24,
                    WHITE,
                    "Time passed: %.2f\n"
                    "Target time: %.2f\n"
                    "Iteration %d/%d\n"
                    "List size: %ld",
                    GAME_TIME_PASSED,
                    duration,
                    iter + 1,
                    iter_count,
                    tracker->objListLen);

        // RunActionList(tracker);

        RunThreads(mtDataWrap);
        CollectThreads(mtDataWrap);
        for (unsigned long i = 0; i < tracker->objListLen; i++) {
            UpdateObjectPos(tracker->objList[i]);
        }
        RunWorldRender(tracker);

        RunScreenRender(tracker);
        PlayerRuntimeControlls(tracker);

        EndDrawing();
        LAST_FRAME_TIME = GetFrameTime();
        GAME_TIME_PASSED += LAST_FRAME_TIME;
    }

    ASSERT(fCutOff(asteroids[0]->objPtr->speed.x, 3) ==
           -fCutOff(small_speed / asteroids[0]->collider.mass, 3));

    ASSERT(asteroids[1]->objPtr->speed.x == 0);

    ASSERT(asteroids[2]->objPtr->speed.x == -asteroids[0]->objPtr->speed.x);

    ASSERT(asteroids[3]->objPtr->speed.x == -asteroids[1]->objPtr->speed.x);

    DeleteTracker(tracker);
    MTCleanupAndFree(mtDataWrap);

    return 0;
}

int TestCollisionVertical(int iter, int iter_count) {
    objTracker *tracker = InitTracker();
    GAME_TIME_PASSED    = 0;
    objWrap *asteroids[4];
    DEBUG(VISUAL_DEBUG = true;)
    const float duration = 1;
    float small_speed    = 90;

    struct mt_data_wrap *mtDataWrap = InitMT(tracker);

    tracker->playerCamera.zoom   = .5f;
    tracker->playerCamera.target = (Vector2){ 400, 700 };
    asteroids[0] =
        CreateAsteroid(tracker, (Vector2){ 600, 300 }, (Vector2){ 0, 0 }, 0, 2);
    asteroids[1] = CreateAsteroid(
        tracker, (Vector2){ 600, 500 }, (Vector2){ 0, -small_speed }, 0, 1);
    asteroids[2] =
        CreateAsteroid(tracker, (Vector2){ 900, 500 }, (Vector2){ 0, 0 }, 0, 2);
    asteroids[3] = CreateAsteroid(
        tracker, (Vector2){ 900, 300 }, (Vector2){ 0, small_speed }, 0, 1);

    while (GAME_TIME_PASSED < duration && !WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(BLACK);

        CleanupMemory(tracker);
        SortListByX(tracker);

        DisplayText((Vector2){ 700, 20 },
                    24,
                    WHITE,
                    "Time passed: %.2f\n"
                    "Target time: %.2f\n"
                    "Iteration %d/%d\n"
                    "List size: %ld",
                    GAME_TIME_PASSED,
                    duration,
                    iter + 1,
                    iter_count,
                    tracker->objListLen);

        // RunActionList(tracker);

        RunThreads(mtDataWrap);
        CollectThreads(mtDataWrap);
        for (unsigned long i = 0; i < tracker->objListLen; i++) {
            UpdateObjectPos(tracker->objList[i]);
        }
        RunWorldRender(tracker);

        RunScreenRender(tracker);
        PlayerRuntimeControlls(tracker);

        EndDrawing();
        LAST_FRAME_TIME = GetFrameTime();
        GAME_TIME_PASSED += GetFrameTime();
    }

    ASSERT(fCutOff(asteroids[0]->objPtr->speed.y, 3) ==
           -fCutOff(small_speed / asteroids[0]->collider.mass, 3));

    ASSERT(asteroids[1]->objPtr->speed.y == 0);

    ASSERT(asteroids[2]->objPtr->speed.y == -asteroids[0]->objPtr->speed.y);

    ASSERT(asteroids[3]->objPtr->speed.y == -asteroids[1]->objPtr->speed.y);

    DeleteTracker(tracker);
    MTCleanupAndFree(mtDataWrap);

    return 0;
}

int TestCollisionCos(int iter, int iter_count) {
    objTracker *tracker = InitTracker();
    GAME_TIME_PASSED    = 0;
    objWrap *asteroids[4];
    DEBUG(VISUAL_DEBUG = true;)
    const float duration = 1;

    struct mt_data_wrap *mtDataWrap = InitMT(tracker);

    tracker->playerCamera.zoom   = .2f;
    tracker->playerCamera.target = (Vector2){ 400, 700 };
    asteroids[0]                 = CreateAsteroid(
        tracker, (Vector2){ 400, 200 }, (Vector2){ -120, 90 }, 0, 1);
    asteroids[1] = CreateAsteroid(
        tracker, (Vector2){ 200, 400 }, (Vector2){ 120, -90 }, 0, 1);

    while (GAME_TIME_PASSED < duration && !WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(BLACK);

        CleanupMemory(tracker);
        SortListByX(tracker);

        DisplayText((Vector2){ 700, 20 },
                    24,
                    WHITE,
                    "Time passed: %.2f\n"
                    "Target time: %.2f\n"
                    "Iteration %d/%d\n"
                    "List size: %ld",
                    GAME_TIME_PASSED,
                    duration,
                    iter + 1,
                    iter_count,
                    tracker->objListLen);

        // RunActionList(tracker);

        RunThreads(mtDataWrap);
        CollectThreads(mtDataWrap);
        for (unsigned long i = 0; i < tracker->objListLen; i++) {
            UpdateObjectPos(tracker->objList[i]);
        }
        RunWorldRender(tracker);

        RunScreenRender(tracker);
        PlayerRuntimeControlls(tracker);

        EndDrawing();
        LAST_FRAME_TIME = GetFrameTime();
        GAME_TIME_PASSED += GetFrameTime();
    }

    ASSERT(fCutOff(asteroids[0]->objPtr->speed.y, 0) == -95.f);

    ASSERT(fCutOff(asteroids[1]->objPtr->speed.y, 0) == 95.f);

    DeleteTracker(tracker);
    MTCleanupAndFree(mtDataWrap);

    return 0;
}

int TestBigUnit(int iter, int iter_count) {
    objTracker *tracker = InitTracker();
    GAME_TIME_PASSED    = 0;
    objWrap *asteroidsCos[2];
    objWrap *asteroidsVert[4];
    objWrap *asteroidsHor[4];
    DEBUG(VISUAL_DEBUG = true;) float small_speed = 90;

    Vector2 start_pos    = { 0, 0 };
    const float duration = 1;

    struct mt_data_wrap *mtDataWrap = InitMT(tracker);

    tracker->playerCamera.zoom   = .2f;
    tracker->playerCamera.target = (Vector2){ 400, 700 };
    asteroidsCos[0] =
        CreateAsteroid(tracker,
                       (Vector2){ start_pos.x + 400, start_pos.y + 200 },
                       (Vector2){ -120, 90 },
                       0,
                       1);
    asteroidsCos[1] =
        CreateAsteroid(tracker,
                       (Vector2){ start_pos.x + 200, start_pos.y + 400 },
                       (Vector2){ 120, -90 },
                       0,
                       1);
    start_pos.x += 1000;
    asteroidsVert[0] =
        CreateAsteroid(tracker,
                       (Vector2){ start_pos.x + 600, start_pos.y + 300 },
                       (Vector2){ 0, 0 },
                       0,
                       2);
    asteroidsVert[1] =
        CreateAsteroid(tracker,
                       (Vector2){ start_pos.x + 600, start_pos.y + 500 },
                       (Vector2){ 0, -small_speed },
                       0,
                       1);
    asteroidsVert[2] =
        CreateAsteroid(tracker,
                       (Vector2){ start_pos.x + 900, start_pos.y + 500 },
                       (Vector2){ 0, 0 },
                       0,
                       2);
    asteroidsVert[3] =
        CreateAsteroid(tracker,
                       (Vector2){ start_pos.x + 900, start_pos.y + 300 },
                       (Vector2){ 0, small_speed },
                       0,
                       1);

    start_pos.x += 1000;
    asteroidsHor[0] =
        CreateAsteroid(tracker,
                       (Vector2){ start_pos.x + 300, start_pos.y + 600 },
                       (Vector2){ 0, 0 },
                       0,
                       2);
    asteroidsHor[1] =
        CreateAsteroid(tracker,
                       (Vector2){ start_pos.x + 500, start_pos.y + 600 },
                       (Vector2){ -small_speed, 0 },
                       0,
                       1);
    asteroidsHor[2] =
        CreateAsteroid(tracker,
                       (Vector2){ start_pos.x + 500, start_pos.y + 900 },
                       (Vector2){ 0, 0 },
                       0,
                       2);
    asteroidsHor[3] =
        CreateAsteroid(tracker,
                       (Vector2){ start_pos.x + 300, start_pos.y + 900 },
                       (Vector2){ small_speed, 0 },
                       0,
                       1);

    while (GAME_TIME_PASSED < duration && !WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(BLACK);

        CleanupMemory(tracker);
        SortListByX(tracker);

        DisplayText((Vector2){ 700, 20 },
                    24,
                    WHITE,
                    "Time passed: %.2f\n"
                    "Target time: %.2f\n"
                    "Iteration %d/%d\n"
                    "List size: %ld",
                    GAME_TIME_PASSED,
                    duration,
                    iter + 1,
                    iter_count,
                    tracker->objListLen);

        // RunActionList(tracker);

        RunThreads(mtDataWrap);
        CollectThreads(mtDataWrap);
        for (unsigned long i = 0; i < tracker->objListLen; i++) {
            UpdateObjectPos(tracker->objList[i]);
        }
        RunWorldRender(tracker);

        RunScreenRender(tracker);
        PlayerRuntimeControlls(tracker);

        EndDrawing();
        LAST_FRAME_TIME = GetFrameTime();
        GAME_TIME_PASSED += GetFrameTime();
    }

    ASSERT(fCutOff(asteroidsCos[0]->objPtr->speed.y, 0) == -95.f);

    ASSERT(fCutOff(asteroidsCos[1]->objPtr->speed.y, 0) == 95.f);

    ASSERT(fCutOff(asteroidsVert[0]->objPtr->speed.y, 3) ==
           -fCutOff(small_speed / asteroidsVert[0]->collider.mass + 5, 3));

    ASSERT(asteroidsVert[1]->objPtr->speed.y == 5);

    ASSERT(asteroidsVert[2]->objPtr->speed.y ==
           -asteroidsVert[0]->objPtr->speed.y);

    ASSERT(asteroidsVert[3]->objPtr->speed.y ==
           -asteroidsVert[1]->objPtr->speed.y);

    ASSERT(fCutOff(asteroidsHor[0]->objPtr->speed.x, 0) ==
           -fCutOff((small_speed + 5) / asteroidsHor[0]->collider.mass, 0));

    ASSERT(asteroidsHor[1]->objPtr->speed.x == 1);

    ASSERT(asteroidsHor[2]->objPtr->speed.x ==
           -asteroidsHor[0]->objPtr->speed.x);

    ASSERT(asteroidsHor[3]->objPtr->speed.x ==
           -asteroidsHor[1]->objPtr->speed.x);

    DeleteTracker(tracker);
    MTCleanupAndFree(mtDataWrap);

    return 0;
}

// Collision testing
