#include "drawing.h"

// Adjusts scale of player geometry as requested
PlayerShape ResizePlayerShape(PlayerShape pShape);

// Draws the player based on it's *player.shape.points* array
int DrawObject(PlayerStruct *player);
