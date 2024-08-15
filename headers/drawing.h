#ifndef DRAWING_H_
#define DRAWING_H_

// Adjusts scale of player geometry as requested
PlayerShape ResizePlayerShape(PlayerShape pShape);

// Draws the player based on it's *player.shape.points* array
int DrawObject(PlayerStruct *player);

#endif // DRAWING_H_
