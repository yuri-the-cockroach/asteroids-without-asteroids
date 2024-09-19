#include "main.h"

#include <err.h>
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/gamelogic.h"
#include "src/objecthandler.h"
#include "src/objectlogic.h"
#include "src/syslogic.h"

const unsigned int BASE_ACCELL = 5;
const unsigned int BASE_ROTATE = 5;

// Retuns prefilled ObjectStruct with default data

int main() {
    signal(SIGINT, SigIntHandler);

    const int screenWidth = 1280;
    const int screenHight = 1080;

    int fpsTarget = 120;

    InitWindow(screenWidth, screenHight, "asteroids without asteroids");

    SetWindowPosition(2560 + 900 - screenWidth, 0);
    SetWindowMinSize(10, 10);
    SetWindowMaxSize(8192, 8192);

    SetTargetFPS(fpsTarget);

    ShapeStruct PLAYER_SHAPE =
        InitShape(PLAYER_SHAPE_POINTS,
                  sizeof(PLAYER_SHAPE_POINTS) / sizeof(Vector2),
                  0.5);
    ShapeStruct ASTEROID_SHAPE =
        InitShape(ASTEROID_SHAPE_POINTS,
                  sizeof(ASTEROID_SHAPE_POINTS) / sizeof(Vector2),
                  1);

    /*  ---------------------<Object Init>--------------------  */

    ObjectTracker tracker = InitTracker();
    CreatePlayer(&tracker, (Vector2){ 200, 200 }, 5, 5);
    CreateAsteroid(&tracker, (Vector2){ 300, 300 }, (Vector2){ 6, 9 }, 5);
    CreateAsteroid(&tracker, (Vector2){ 400, 300 }, (Vector2){ 7, 9 }, -2);

    printf("sizeof(PLAYER_SHAPE): %ld\n", sizeof(PLAYER_SHAPE.arrayLength));
    printf("sizeof(PLAYER_SHAPE): %ld\n", sizeof(Vector2));
    while (!WindowShouldClose()) {
        // Adjust moving speed for current frametime, so you don't get faster as
        // fps goes up
        /* KeyboardHandler(&player); // All the keybindings and movement */

        /* sprintf(currentRotation, */
        /*         "pos x: %f y: %f", */
        /*         player.position.x, */
        /*         player.position.y); */
        /* UpdateObjectPos(&asteroid); */
        /* UpdateObjectPos(&player); */

        RunActionList(&tracker);

        if (IsKeyPressed('9'))
            SetTargetFPS(fpsTarget = 75);
        if (IsKeyPressed('0'))
            SetTargetFPS(fpsTarget = 0);
        if (IsKeyPressed('='))
            SetTargetFPS(fpsTarget += 10);
        if (IsKeyPressed('-'))
            SetTargetFPS(fpsTarget -= 10);
        BeginDrawing();

        ClearBackground(BLACK);

        DrawFPS(400, 0);
        DrawAllFromDrawList(&tracker);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
