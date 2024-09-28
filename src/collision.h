#ifndef COLLISION_H_
#define COLLISION_H_

#include "structs.h"
#include "logger.h"

#define BOUCE_CONSTANT 100

/* Check anything collides with this object
 Return either TRUE or FALSE */
bool FindAnyCollision(ObjectTracker *tracker, ObjectWrap *first);

/* Collision checking */
void FindCollisions(ObjectTracker *tracker, ObjectWrap *wrap);

/* Changes object's rotation taking it's mass into account */
void ApplyMassBasedRandRotation(ObjectWrap *wrap);

/* Check if two objects collide */
bool CheckIfCollide(ObjectWrap *first, ObjectWrap *second);

/* Collision response function */
void Bounce(ObjectTracker *tracker, ObjectWrap *first, ObjectWrap *second);

/* If object callided with a projectile */
void GetShot(ObjectTracker *tracker, ObjectWrap *first, ObjectWrap *second);

Collider InitCollider(float sizeMult, void (*ActionOnCollision)(ObjectTracker *tracker, ObjectWrap *first, ObjectWrap *second));

// Push to the end of list
// Return value can be -1 unable to push
int Push(Collider *parent, ObjectWrap *wrap);

// Find the entry in the list
// Return value can be -1 if not found
int FindInList(Collider *parent, ObjectWrap *wrap);

// Pop entry from the list
// Return value CAN BE NULL if not found
ObjectWrap* Pop(Collider *parent, int index);

// Clear the list
int ClearList(Collider *parent);

// Clean all the lists
int CleanupLists(ObjectTracker *tracker);

int UpdateCollider(ObjectWrap *wrap);

void PlayerCollision(ObjectTracker *tracker, ObjectWrap *player, ObjectWrap *offender);

#endif // COLLISION_H_
