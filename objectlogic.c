#include <raylib.h>
#include <raymath.h>
#include "headers/playerlogic.h"

void UpdatePlayerPos(PlayerStruct *player) {
    player->position.x = player->position.x + player->speed.x * GetFrameTime();
    player->position.y = player->position.y + player->speed.y * GetFrameTime();
}

void OnAccelerate(PlayerStruct *player, float speed) {
    player->speed.x = player->speed.x + player->shape.points[0].x * speed;
    player->speed.y = player->speed.y + player->shape.points[0].y * speed;
}

void UpdatePlayerHeading(PlayerStruct *player) {
    player->heading = atan2(player->shape.points[0].x, player->shape.points[0].y);
}


PlayerStruct TransformPlayerMatrix(PlayerStruct * player, float rotationAngle) {
    PlayerStruct tempPl = *player;

    unsigned int arrayLength = sizeof(player->shape.points) / sizeof(Vector2);
    for (unsigned int current = 0; current < arrayLength; current++) {
        tempPl.shape.points[current].x = ( player->shape.points[current].x * cos(rotationAngle) ) - ( player->shape.points[current].y * sin(rotationAngle) );
        tempPl.shape.points[current].y = ( player->shape.points[current].x * sin(rotationAngle) ) + ( player->shape.points[current].y * cos(rotationAngle) );
    }

    return tempPl;
}

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

PlayerShape ResizePlayerShape(PlayerShape pShape) {
    unsigned int arrayLength = sizeof(pShape.points) / sizeof(Vector2);

    PlayerShape tempPShape;

    for (unsigned int current = 0; current < arrayLength; current++) {
        tempPShape.points[current].x = round(pShape.points[current].x * pShape.size);
        tempPShape.points[current].y = round(pShape.points[current].y * pShape.size);
    }
    return tempPShape;
}

PlayerStruct InitPlayer() {
    return (PlayerStruct){
        BASE_ACCELL,            // acceleration speed
        BASE_ROTATE,            // Rotation speed
        0,                      // Starter heading
        (Vector2){ 0, 0 },      // Starting position of the player
        (Vector2){ 0, 0 },      // Starting speed of the player
        ResizePlayerShape(PLAYER_BASE_SHAPE) // Shape of the player
    };
}
