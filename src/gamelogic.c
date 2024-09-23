#include "gamelogic.h"
#include "objecthandler.h"
#include "structs.h"
#include "utils.h"
#include <raylib.h>
#include <stdio.h>

void OnPlayerAccellerate(ObjectStruct *object, float speed) {
    object->speed.x += object->shape.points[0].x * (speed * GetFrameTime());
    object->speed.y += object->shape.points[0].y * (speed * GetFrameTime());
}

void DebugingKeyHandler(ObjectTracker *tracker) {

    if (IsKeyPressed('T')) {
        printf("Breakpoint says hi\n");
    }

    if (IsKeyPressed('O')) {
        tracker->objList[0]->objPtr->position = (Vector2){ 0, 0 };
    }

    if (IsKeyDown(' ')) {
        if (GetTimeMS() - lastShot > 1000 / RATE_OF_FIRE) {
            lastShot = GetTimeMS();
            CreateProjectile(tracker, tracker->objList[0]);
        }
    }

    if (IsKeyPressed('P')) {
        FPS_TARGET = FPS_TARGET == 75 ? 0 : 75;
        SetTargetFPS(FPS_TARGET);
    }

    if (IsKeyPressed('1'))
        CreateAsteroid(tracker, (Vector2){ 300, 300 }, (Vector2){ 0, 0 }, 0, 1);

    if (IsKeyPressed('2')) {
        CreateAsteroid(
            tracker, (Vector2){ 400, 300 }, (Vector2){ 30, 120 }, 0, 1);
        CreateAsteroid(
            tracker, (Vector2){ 500, 600 }, (Vector2){ -30, -120 }, 0, 1);
    }

    if (IsKeyPressed('3')) {

        CreateAsteroid(
            tracker, (Vector2){ 400, 300 }, (Vector2){ 30, 60 }, 0, 1);
        CreateAsteroid(
            tracker, (Vector2){ 600, 600 }, (Vector2){ -30, -60 }, 0, 1);
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

        CreateAsteroid(tracker, (Vector2){ 400, 300 }, (Vector2){ 0, 0 }, 0, 1);
        CreateAsteroid(
            tracker, (Vector2){ 900, 300 }, (Vector2){ -120, 0 }, 0, 2);
    }

    if (IsKeyPressed('9')) {
        for (unsigned int i = 0; i < MAX_OBJECT_COUNT - 1; i++) {
            CreateAsteroid(tracker,
                           (Vector2){ GetRandomFloat(0, (float)SCREEN_WIDTH),
                                      GetRandomFloat(0, (float)SCREEN_HEIGHT) },

                           (Vector2){ GetRandomFloat(-100, 100),
                                      GetRandomFloat(-100, 100) },
                           GetRandomFloat(-5, 5),
                           1);
        }
    }

    if (IsKeyPressed('0')) {
        unsigned long iterations = tracker->objListLen;
        if (iterations > 1) {
            for (unsigned long i = iterations - 1; i > 0; i--) {
                DeleteTrackedObject(tracker, i);
            }
        }
    }

    if (IsKeyPressed('='))
        CreateAsteroid(
            tracker,
            (Vector2){ GetRandomFloat(0, (float)SCREEN_WIDTH),
                       GetRandomFloat(0, (float)SCREEN_HEIGHT) },
            (Vector2){ GetRandomFloat(-100, 100), GetRandomFloat(-100, 100) },
            GetRandomFloat(-5, 5),
            GetRandomFloat(0.5, 2));

    if (IsKeyPressed('-')) {
        if (tracker->objListLen > 1)
            tracker->objList[tracker->objListLen - 1]->request = DELETE;
    }
}

void KeyboardHandler(ObjectStruct *object) {
    if (IsKeyDown('W'))
        OnPlayerAccellerate(object, PLAYER_MOVE_SPEED);
    if (IsKeyDown('S'))
        OnPlayerAccellerate(object, -PLAYER_MOVE_SPEED);
    if (IsKeyDown('D'))
        RotateObject(object, PLAYER_ROTATION_SPEED);
    if (IsKeyDown('A'))
        RotateObject(object, -PLAYER_ROTATION_SPEED);
}
