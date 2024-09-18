#include "headers/main.h"

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

    ShapeStruct PLAYER_SHAPE = InitShape(PLAYER_SHAPE_POINTS, sizeof(PLAYER_SHAPE_POINTS) / sizeof(Vector2), 0.5);
    ShapeStruct ASTEROID_SHAPE = InitShape(ASTEROID_SHAPE_POINTS, sizeof(ASTEROID_SHAPE_POINTS) / sizeof(Vector2), 1);


/*  ---------------------<Object Init>--------------------  */


    ObjectStruct asteroid = InitObject(ASTEROID_SHAPE);
    asteroid.position = (Vector2){300, 300};

    asteroid.speed = (Vector2){6, 9};

    ObjectStruct player = InitObject(PLAYER_SHAPE);
    player.position = (Vector2){ 200, 200 };
    player.moveSpeed = 5;
    player.rotateSpeed = 5;

    char * currentRotation = malloc(sizeof(char) * 64);

    printf("player array length: %d\n", player.shape.arrayLength);
    printf("sizeof(PLAYER_SHAPE): %ld\n", sizeof(PLAYER_SHAPE.arrayLength));
    printf("sizeof(PLAYER_SHAPE): %ld\n", sizeof(Vector2));
    while (!WindowShouldClose()) {
         // Adjust moving speed for current frametime, so you don't get faster as fps goes up
        KeyboardHandler(&player); // All the keybindings and movement

        sprintf(currentRotation, "pos x: %f y: %f", player.position.x, player.position.y);
        UpdateObjectPos(&asteroid);
        UpdateObjectPos(&player);

        if (IsKeyPressed('9')) SetTargetFPS(fpsTarget = 75);
        if (IsKeyPressed('0')) SetTargetFPS(fpsTarget = 0);
        if (IsKeyPressed('=')) SetTargetFPS(fpsTarget += 10);
        if (IsKeyPressed('-')) SetTargetFPS(fpsTarget -= 10);
        BeginDrawing();


            ClearBackground(BLACK);

            /* DrawRectangle(object.position.x, object.position.y, 100, 100, RED); */
            DrawObject(&asteroid);
            DrawObject(&player);
            DrawFPS(400, 0);
            DrawText(currentRotation, 10, 10, 24, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
