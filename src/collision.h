#ifndef COLLISION_H_
#define COLLISION_H_

#include "logger.h"
#include "structs.h"

#define BOUCE_CONSTANT 1

// Check anything collides with this object
// Return either TRUE or FALSE
bool FindAnyCollision(objTracker *tracker, objWrap *first);

// Sort the list by X coordinate
void SortListByX(objTracker *tracker);

void MaybeFindCollisions(objTracker *tracker, unsigned long index);
// An algorithm that shoud in theory be a lot faster
void FastFindCollisions(objTracker *tracker, unsigned long index);

// Find an object that ocupies pos
objWrap *FindCollisionPos(objTracker *tracker, Vector2 pos);

// Changes object's rotation taking it's mass into account
void ApplyMassBasedRandRotation(objWrap *wrap);

// Check if two objects collide
bool CheckIfCollide(objWrap *first, objWrap *second);

// Collision response function
void Bounce(objTracker *tracker, objWrap *first, objWrap *second);

// If object callided with a projectile
void GetShot(objTracker *tracker, objWrap *first, objWrap *second);

collider InitCollider(float sizeMult,
                      void (*ActionOnCollision)(objTracker *tracker,
                                                objWrap *first,
                                                objWrap *second));

int UpdateCollider(objWrap *wrap);

void PlayerCollision(objTracker *tracker, objWrap *player, objWrap *offender);

#endif // COLLISION_H_
