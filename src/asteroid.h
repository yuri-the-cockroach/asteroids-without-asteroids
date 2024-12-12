#ifndef ASTEROID_H_
#define ASTEROID_H_
#include "structs.h"
#include "autils.h"
#include "objectlogic.h"
#include "objecthandler.h"

// Will spawn asteroid in such a way, that it won't collide with anything
objWrap *AsteroidSafeSpawn(objTracker *tracker);

// Generates a surface for an asteroid
Vector2 *GenerateAsteroidShape(void);

// A wrapper funciton for AddWrapToList, to create an asteroid
objWrap *CreateAsteroid(objTracker *tracker, Vector2 initPosition,
                    Vector2 initSpeed, float constRotationSpeed, float size);

int Separate(objTracker *tracker, objWrap *parent);

#endif // ASTEROID_H_
