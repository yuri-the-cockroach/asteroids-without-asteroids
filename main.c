
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <rcamera.h>
#include <signal.h>
#include <err.h>
#include <stdbool.h>

/* --------------------- Structs -------------------- */

struct playerShape {
    Vector2 points[4];

} typedef playerShape;

struct player {
    Vector2 position;
    playerShape shape;
} typedef playerStruct;


/* --------------------- Functions -------------------- */

void SigIntHandler(int sig) {
    err(SIGINT, "Got a signal interupt from console. Exiting now\n");
}

void KeyboardHandler(playerStruct * player, float moveSpeed) {

    if (IsKeyDown('W')) player->position.y -= moveSpeed;
    if (IsKeyDown('S')) player->position.y += moveSpeed;
    if (IsKeyDown('D')) player->position.x += moveSpeed;
    if (IsKeyDown('A')) player->position.x -= moveSpeed;
}

void DrawPlayer(playerStruct * player) {
    int prevPoint = 0;
    for (int curPoint = 0; curPoint < sizeof(player->shape.points) / sizeof(Vector2); curPoint++){
        if (curPoint == prevPoint) continue;
        DrawLineEx (
            (Vector2) { // Draw from x/y
                player->shape.points[prevPoint].x + player->position.x,
                player->shape.points[prevPoint].y + player->position.y
            },
            (Vector2) { // Draw to x/y
                player->shape.points[curPoint].x + player->position.x,
                player->shape.points[curPoint].y + player->position.y
            },
            2.0, // Thikness of the line
            WHITE // Color of the line
        );
        prevPoint = curPoint;

        // Draw the final line to finish the form. I'm too lazy to do this less janky
        if (curPoint == sizeof(player->shape.points) / sizeof(Vector2) - 1) {
            prevPoint = 0;
            DrawLineEx (
                (Vector2) { // Draw from x/y
                    player->shape.points[prevPoint].x + player->position.x,
                    player->shape.points[prevPoint].y + player->position.y
                },
                (Vector2) { // Draw to x/y
                    player->shape.points[curPoint].x + player->position.x,
                    player->shape.points[curPoint].y + player->position.y
                },
                1.0, // Thikness of the line
                WHITE // Color of the line
            );
        }
    }
}


int main() {
    signal(SIGINT, SigIntHandler);

    const int screenWidth = 1280;
    const int screenHight = 1020;

    const float baseMoveSpeed = 50;
    float moveSpeed = 0;

    int fpsTarget = 120;


    InitWindow(screenWidth, screenHight, "Something something artillery");

    SetWindowPosition(2560 + 900 - screenWidth, 0);
    SetWindowMinSize(10, 10);
    SetWindowMaxSize(8192, 8192);

    Vector2 cubePositionOrSomething = { 0, 0 };

    SetTargetFPS(fpsTarget);

    char isFocused[128] = "";


    /* --------------------- Player Init -------------------- */

    playerStruct player = (playerStruct){
        (Vector2){ 0, 0 },
        {
            (Vector2){  0, -100 },
            (Vector2){  50,   50 },
            (Vector2){  0,   0 },
            (Vector2){ -50,   50 }
        }
    };

    while (!WindowShouldClose()) {
        moveSpeed = baseMoveSpeed * GetFrameTime(); // Adjust moving speed for current frametime, so you don't get faster as fps goes up
        KeyboardHandler(&player, moveSpeed); // All the keybindings and movement


        if (IsKeyPressed('=')) SetTargetFPS(fpsTarget += 10);
        if (IsKeyPressed('-')) SetTargetFPS(fpsTarget -= 10);

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            /* DrawRectangle(player.position.x, player.position.y, 100, 100, RED); */
            DrawPlayer(&player);
            DrawFPS(400, 0);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
