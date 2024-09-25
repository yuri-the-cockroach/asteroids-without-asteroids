#include <raylib.h>
#include <stdio.h>

#include "asteroidsutils.h"
#include "gamelogic.h"
#include "objecthandler.h"
#include "structs.h"
#include "asteroidsutils.h"

void OnPlayerAccellerate(ObjectStruct *object, float speed) {
    object->speed.x += object->shape.points[0].x * (speed * GetFrameTime());
    object->speed.y += object->shape.points[0].y * (speed * GetFrameTime());
}

void DebugingKeyHandler(ObjectTracker *tracker) {

    if (IsKeyPressed('T')) {
        GDB_STOP = !GDB_STOP;
        printf("Breakpoint says hi\n");
    }
    if (IsKeyPressed('V')) {
        VISUAL_DEBUG = !VISUAL_DEBUG;
    }

    if (IsKeyPressed('O')) {
        tracker->objList[0]->objPtr->position = (Vector2){ 0, 0 };
        tracker->objList[0]->objPtr->speed = (Vector2){ 0, 0 };
    }

    if (IsKeyDown(' ')) {
        if (GetTimeMS() - lastShot > (long)1e6 / RATE_OF_FIRE) {
            lastShot = GetTimeMS();
            CreateProjectile(tracker, tracker->objList[0]);
        }
    }

    if (IsKeyPressed('P')) {
        PAUSE_GAME = !PAUSE_GAME;
    }

    if (IsKeyPressed('1')) {
        CreateAsteroid(
            tracker, (Vector2){ 300, 600 }, (Vector2){ 0, 0 }, 0, 1);
        CreateAsteroid(
            tracker, (Vector2){ 500, 600 }, (Vector2){ -90, 0 }, 0, 1);


        CreateAsteroid(
            tracker, (Vector2){ 300, 900 }, (Vector2){ 90, 0 }, 0, 1);
        CreateAsteroid(
            tracker, (Vector2){ 500, 900 }, (Vector2){ 0, 0 }, 0, 1);

        CreateAsteroid(
            tracker, (Vector2){ 300, 300 }, (Vector2){ 90, 0 }, 0, 1);
        CreateAsteroid(
            tracker, (Vector2){ 500, 300 }, (Vector2){ -90, 0 }, 0, 1);
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
        /*     tracker, (Vector2){ 500, 600 }, (Vector2){ -90, 0 }, 0, 2); */

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
            (Vector2){
                       GetRandomFloat(tracker->objList[0]->objPtr->position.x - 500, tracker->objList[0]->objPtr->position.x + 500),
                       GetRandomFloat(tracker->objList[0]->objPtr->position.y - 500, tracker->objList[0]->objPtr->position.y + 500) },
            (Vector2){ GetRandomFloat(-100, 100), GetRandomFloat(-100, 100) },
            GetRandomFloat(-5, 5),
            GetRandomFloat(0.5, 2));

    if (IsKeyPressed('-')) {
        if (tracker->objListLen > 1)
            tracker->objList[tracker->objListLen - 1]->request = DELETE;
    }
}


void ShipControlls(ObjectStruct *object) {
    if (IsKeyDown('W'))
        OnPlayerAccellerate(object, PLAYER_MOVE_SPEED);
    if (IsKeyDown('S'))
        OnPlayerAccellerate(object, -PLAYER_MOVE_SPEED);
    if (IsKeyDown('D'))
        RotateObject(object, PLAYER_ROTATION_SPEED);
    if (IsKeyDown('A'))
        RotateObject(object, -PLAYER_ROTATION_SPEED);
}

void KeyboardHandler(ObjectStruct *object) {

    if (IsKeyPressed('C'))
        CAMERA_FOLLOW = !CAMERA_FOLLOW;

    if ( IsMouseButtonDown(1) ) {
        if ( CAMERA_FOLLOW ) CAMERA_FOLLOW = false;
        Vector2 mouseDelta = GetMouseDelta();
        camera.target.x -= mouseDelta.x / camera.zoom;
        camera.target.y -= mouseDelta.y / camera.zoom;
    }

    if ( CAMERA_FOLLOW ) {
        camera.target.x = object->position.x;
        camera.target.y = object->position.y;
    }

    camera.zoom = ClampFloat(camera.zoom + GetMouseWheelMove() / 10, 0.3f, 3);
}
