#ifndef COLLISION_H_
#define COLLISION_H_

#include "structs.h"
#include "logger.h"

#define BOUCE_CONSTANT 100

// Check anything collides with this object
// Return either TRUE or FALSE
bool FindAnyCollision(objTracker *tracker, objWrap *first);

// Sort the list by X coordinate
void SortListByX(objTracker *tracker);

// An algorithm that shoud in theory be a lot faster
void FastFindCollisions(objTracker *tracker, unsigned long index);

// Collision checking
void FindCollisions(objTracker *tracker, objWrap *wrap);

// Changes object's rotation taking it's mass into account
void ApplyMassBasedRandRotation(objWrap *wrap);

// Check if two objects collide
bool CheckIfCollide(objWrap *first, objWrap *second);

// Collision response function
void Bounce(objTracker *tracker, objWrap *first, objWrap *second);

// If object callided with a projectile
void GetShot(objTracker *tracker, objWrap *first, objWrap *second);

collider InitCollider(float sizeMult, void (*ActionOnCollision)(objTracker *tracker, objWrap *first, objWrap *second));

// Push to the end of list
// Return value can be -1 unable to push
int Push(collider *parent, objWrap *wrap);

// Find the entry in the list
// Return value can be -1 if not found
int FindInList(collider *parent, objWrap *wrap);

// Pop entry from the list
// Return value CAN BE NULL if not found
objWrap* Pop(collider *parent, int index);

// Clear the list
int ClearList(collider *parent);

// Clean all the lists
int CleanupLists(objTracker *tracker);

int UpdateCollider(objWrap *wrap);

void PlayerCollision(objTracker *tracker, objWrap *player, objWrap *offender);

#endif // COLLISION_H_
