#ifndef OBJECTHANDLER_H_
#define OBJECTHANDLER_H_

#include <raylib.h>
#include <stdbool.h>
#include "structs.h"

void RunActionList(ObjectTracker *self);

/* Called on a single ObjectWrap* and goes through the update checklist */
void UpdateObj(ObjectTracker *tracker, ObjectWrap *wrap);

/* Takes care of safely deleting the object and cleaning up after it */
void DeleteObjWrap(ObjectWrap *self);


/* Initializes the tracker and returns it */
ObjectTracker InitTracker(void);
ObjectWrap InitWrap(void);

void AddWrapToList(ObjectTracker *self, ObjectWrap *wrap);
void CreatePlayer(ObjectTracker *self, Vector2 initPosition,
                  float size);

void CreateAsteroid(ObjectTracker *self, Vector2 initPosition,
                    Vector2 initSpeed, float constRotationSpeed, float size);

void DeleteObjWrap(ObjectWrap *self);
void DeleteTrackedObject(ObjectTracker *self, ObjectWrap *wrap);

/* ----- Collision checking ----- */
void CheckCollisions(ObjectTracker *tracker, ObjectWrap *wrap);

void Bounce(ObjectWrap *first, ObjectWrap *second);
#endif // OBJECTHANDLER_H_
