#ifndef PLAYERLOGIC_H_
#define PLAYERLOGIC_H_

#include <raylib.h>

const unsigned int BASE_ACCELL = 5;
const unsigned int BASE_ROTATE = 5;

struct PlayerShape {
    float   size;
    Vector2 points[4];
} typedef   PlayerShape;

struct player {
    float       moveSpeed;
    float       rotateSpeed;
    float       heading;
    Vector2     position;
    Vector2     speed;
    PlayerShape shape;
} typedef       PlayerStruct;

const PlayerShape PLAYER_BASE_SHAPE = (PlayerShape) { // Geometry of the player model
    0.5,
    (Vector2){   0, -100 }, // Is always treated as the fronts-most point, used to get the heading
    (Vector2){  50,   50 },
    (Vector2){   0,    0 },
    (Vector2){ -50,   50 }
};

/*
---------------------------------------- Function definitions start here ----------------------------------------
*/


void UpdatePlayerPos(PlayerStruct *player);

// Handles acceleration of the player
void OnAccelerate(PlayerStruct *player, float speed);

// Determens and updates player.heading variable
void UpdatePlayerHeading(PlayerStruct *player);

// Returns player rotated by *player.shape.rotate* degre without mutating it
PlayerStruct TransformPlayerMatrix(PlayerStruct * player, float rotationAngle);

// Rotates provided player by *player.shape.rotate* degree !!Mutating player inplace!!
void MutTransformPlayerMatrix(PlayerStruct * player, float rotationAngle);

// Draws the player based on it's *player.shape.points* array
void DrawPlayer(PlayerStruct * player);

// Adjusts scale of player geometry as requested
PlayerShape ResizePlayerShape(PlayerShape pShape);

// Returns PlayerStruct ready to use with default data initialized
PlayerStruct InitPlayer();
#endif // PLAYERLOGIC_H_
