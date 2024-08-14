
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <rcamera.h>
#include <raymath.h>
#include <signal.h>
#include <err.h>
#include <stdbool.h>

/* --------------------- Structs --------------------- */

struct PlayerShape {
    float   size;
    Vector2 points[4];
} typedef   PlayerShape;

struct player {
    Vector2     position;
    float       heading;
    PlayerShape shape;
} typedef       PlayerStruct;

const PlayerShape PLAYER_BASE_SHAPE = (PlayerShape) { // Geometry of the player model
    0.5,
    (Vector2){   0, -100 }, // Is always treated as the fronts-most point, used to get the heading
    (Vector2){  50,   50 },
    (Vector2){   0,    0 },
    (Vector2){ -50,   50 }
};

/* ----------------- General System Logic ----------------- */

void SigIntHandler(int sig) {
    err(SIGINT, "Got a signal interupt from console. Exiting now\n");
}

/* ----------------- Player Geometry Functions ----------------- */

PlayerShape ResizePlayerShape(PlayerShape pShape) {
    unsigned int arrayLength = sizeof(pShape.points) / sizeof(Vector2);

    PlayerShape tempPShape;

    for (unsigned int current = 0; current < arrayLength; current++) {
        tempPShape.points[current].x = round(pShape.points[current].x * pShape.size);
        tempPShape.points[current].y = round(pShape.points[current].y * pShape.size);
    }
    return tempPShape;
}



// Retuns prefilled PlayerStruct with default data
PlayerStruct InitPlayer() {
    return (PlayerStruct){
        (Vector2){ 0, 0 },      // Starting position of the player
        0,
        ResizePlayerShape(PLAYER_BASE_SHAPE)
    };
}

/* --------------------- Player Logic Functions -------------------- */

void OnAccelerate(PlayerStruct *player, float speed) {
    player->position.x = player->position.x + player->shape.points[0].x * speed;
    player->position.y = player->position.y + player->shape.points[0].y * speed;
}

void UpdatePlayerHeading(PlayerStruct *player) {
    player->heading = atan2(player->shape.points[0].x, player->shape.points[0].y);
}

// Returns player rotated by *player.shape.rotate* degre without mutating it
PlayerStruct TransformPlayerMatrix(PlayerStruct * player, float rotationAngle) {
    PlayerStruct tempPl = *player;

    unsigned int arrayLength = sizeof(player->shape.points) / sizeof(Vector2);
    for (unsigned int current = 0; current < arrayLength; current++) {
        tempPl.shape.points[current].x = ( player->shape.points[current].x * cos(rotationAngle) ) - ( player->shape.points[current].y * sin(rotationAngle) );
        tempPl.shape.points[current].y = ( player->shape.points[current].x * sin(rotationAngle) ) + ( player->shape.points[current].y * cos(rotationAngle) );
    }

    return tempPl;
}


// Rotates provided player by *player.shape.rotate* degree !!Mutating player inplace!!
void MutTransformPlayerMatrix(PlayerStruct * player, float rotationAngle) {
    PlayerStruct tempPl = *player;

    unsigned int arrayLength = sizeof(player->shape.points) / sizeof(Vector2);
    for (unsigned int current = 0; current < arrayLength; current++) {
        tempPl.shape.points[current].x = ( player->shape.points[current].x * cos(rotationAngle) ) - ( player->shape.points[current].y * sin(rotationAngle) );
        tempPl.shape.points[current].y = ( player->shape.points[current].x * sin(rotationAngle) ) + ( player->shape.points[current].y * cos(rotationAngle) );
    }
    *player = tempPl;
}

void DrawPlayer(PlayerStruct * player) {
    int prevPoint = 0;
    unsigned int arrayLength = sizeof(player->shape.points) / sizeof(Vector2);

    for (unsigned int curPoint = 1; curPoint < arrayLength; curPoint++){
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

    }

    // Draw the final line to finish the form. I'm too lazy to do this less janky
    DrawLineEx
    (
        (Vector2){ // Draw from x/y
            player->shape.points[0].x + player->position.x,
            player->shape.points[0].y + player->position.y
        },
        (Vector2){ // Draw to x/y
            player->shape.points[arrayLength - 1].x + player->position.x,
            player->shape.points[arrayLength - 1].y + player->position.y
        },
        1.0,  // Thikness of the line
        WHITE // Color of the line
    );
}


/* --------------------- Game Logic  -------------------- */

void KeyboardHandler(PlayerStruct * player, float moveSpeed, float rotateSpeed) {

    float adjMoveSpeed = moveSpeed * GetFrameTime();
    float adjRotateSpeed = rotateSpeed * GetFrameTime();
    float rotateBy = 0;

    if (IsKeyDown('W')) OnAccelerate(player, adjMoveSpeed);
    if (IsKeyDown('S')) OnAccelerate(player, -adjMoveSpeed);
    if (IsKeyDown('D')) rotateBy += adjRotateSpeed;
    if (IsKeyDown('A')) rotateBy -= adjRotateSpeed;

    MutTransformPlayerMatrix(player, rotateBy);
}

int main() {
    signal(SIGINT, SigIntHandler);

    const int screenWidth = 1280;
    const int screenHight = 1020;

    float moveSpeed = 10;
    float rotateSpeed = 3;

    int fpsTarget = 120;


    InitWindow(screenWidth, screenHight, "Something something artillery");

    SetWindowPosition(2560 + 900 - screenWidth, 0);
    SetWindowMinSize(10, 10);
    SetWindowMaxSize(8192, 8192);

    Vector2 cubePositionOrSomething = { 0, 0 };

    SetTargetFPS(fpsTarget);

    char isFocused[128] = "";


    /* --------------------- Player Init -------------------- */

    PlayerStruct player = InitPlayer();
    player.position = (Vector2){ 200, 200 };

    while (!WindowShouldClose()) {
         // Adjust moving speed for current frametime, so you don't get faster as fps goes up
        KeyboardHandler(&player, moveSpeed, rotateSpeed); // All the keybindings and movement


        if (IsKeyPressed('9')) SetTargetFPS(fpsTarget = 75);
        if (IsKeyPressed('0')) SetTargetFPS(fpsTarget = 0);
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
