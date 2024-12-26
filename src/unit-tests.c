#include <raylib.h>

#include "asteroid.h"
#include "collision.h"
#include "gamelogic.h"
#include "objecthandler.h"
#include "render.h"
#include "structs.h"
#include "unit-tests.h"

#pragma GCC diagnostic ignored "-Wfloat-equal"

int TestCollision() {
    objTracker *tracker = InitTracker();
    GAME_TIME_PASSED    = 0;
    objWrap *asteroids[4];

    float small_speed = 90;

#ifdef MT_ENABLED
    struct mt_data_wrap *mtDataWrap = InitMT(tracker);
#endif // MT_ENABLED

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

    while (GAME_TIME_PASSED < 2.f) {

        BeginDrawing();
        ClearBackground(BLACK);

#ifdef MT_ENABLED
        CollectThreads(mtDataWrap);
#endif // MT_ENABLED
        RunActionList(tracker);
#ifdef MT_ENABLED
        RunThreads(mtDataWrap);
#endif // MT_ENABLED
        RunScreenRender(tracker);
        RunWorldRender(tracker);
        PlayerRuntimeControlls(tracker);
        EndDrawing();
        GAME_TIME_PASSED += GetFrameTime();
    }

    ASSERT((fCutOff(asteroids[0]->objPtr->speed.x, 3) ==
            -fCutOff(small_speed / asteroids[0]->collider.mass, 3)),
           "%f == %f",
           fCutOff(asteroids[0]->objPtr->speed.x, 3),
           -fCutOff(small_speed / asteroids[0]->collider.mass, 3));

    ASSERT((asteroids[1]->objPtr->speed.x == 0),
           "%f == %f",
           asteroids[1]->objPtr->speed.x,
           0);

    ASSERT((asteroids[2]->objPtr->speed.x == -asteroids[0]->objPtr->speed.x),
           "%f == %f",
           asteroids[2]->objPtr->speed.x,
           -asteroids[0]->objPtr->speed.x);

    ASSERT((asteroids[3]->objPtr->speed.x == -asteroids[1]->objPtr->speed.x),
           "%f == %f",
           asteroids[3]->objPtr->speed.x,
           -asteroids[1]->objPtr->speed.x);

    DeleteTracker(tracker);
    return 0;
}

// Collision testing
