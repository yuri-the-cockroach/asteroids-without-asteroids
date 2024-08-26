#ifndef PLAYERLOGIC_H_
#define PLAYERLOGIC_H_

#include <raylib.h>
#include "gamelogic.h"

struct player {
    BasePhysObject physObject;
    BaseShape    shape;
} typedef PlayerStruct;

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

// Handles input
void KeyboardHandler(PlayerStruct *player);

// Handles acceleration of the player
void OnAccelerate(PlayerStruct *player, float speed);

// Returns PlayerStruct ready to use with default data initialized
PlayerStruct InitPlayer();
#endif // PLAYERLOGIC_H_
