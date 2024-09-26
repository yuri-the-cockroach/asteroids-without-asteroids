#ifndef OBJECTHANDLER_H_
#define OBJECTHANDLER_H_

#include "structs.h"

#include "asteroidsutils.h"
#include "collision.h"
#include "gamelogic.h"
#include "objectlogic.h"
#include "render.h"

/* Go through the list of tracked objects and call UpdateObj function on them */
void RunActionList(ObjectTracker *tracker);

/* Called on a single ObjectWrap* and goes through the update checklist */
void UpdateObj(ObjectTracker *tracker, ObjectWrap *wrap);

/* Takes care of safely deleting the object and cleaning up after it */
void DeleteObjWrap(ObjectWrap *wrap);

/* Initializes the tracker and returns it */
ObjectTracker *InitTracker(void);

/* Initialize an object wrapper that stores general data */
ObjectWrap InitWrap(void);

/* Adds the project wrapper to the list of tracked objects
 * This function is not ment to be called from the top level
 * It is ment to be called by a wrapper function that makes a specific object */
int AddWrapToList(ObjectTracker *tracker, ObjectWrap *wrap);

/* A wrapper funciton for AddWrapToList, to create a player */
void CreatePlayer(ObjectTracker *tracker, Vector2 initPosition,
                  float size);

/* A wrapper funciton for AddWrapToList, to create an asteroid */
void CreateAsteroid(ObjectTracker *tracker, Vector2 initPosition,
                    Vector2 initSpeed, float constRotationSpeed, float size);

/* Creates a projectile that inherits from it's parent */
void CreateProjectile(ObjectTracker *tracker, ObjectWrap *parent);

/* Destructor for ObjectWrap */
void DeleteObjWrap(ObjectWrap *wrap);

/* Wrapper for ObjectWrapper destructor that hanldes cleanup from the tracking list  */
void DeleteTrackedObject(ObjectTracker *tracker, unsigned long index);

/* Deinit tracker and everything in it */
void DeleteTracker(ObjectTracker *tracker);


#endif // OBJECTHANDLER_H_
