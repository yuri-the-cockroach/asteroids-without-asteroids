#ifndef OBJECTHANDLER_H_
#define OBJECTHANDLER_H_

#include "structs.h"

// Go through the list of tracked objects and call UpdateObj function on them
void RunActionList(objTracker *tracker);

// Takes care of safely deleting the object and cleaning up after it
void DeleteObjWrap(objWrap *wrap);

// Initializes the tracker and returns it
objTracker *InitTracker(void);

// Initialize an object wrapper that stores general data
objWrap InitWrap(void);

// Adds the project wrapper to the list of tracked objects
// This function is not ment to be called from the top level
// It is ment to be called by a wrapper function that makes a specific object
int AddWrapToList(objTracker *tracker, objWrap *wrap);

// A wrapper funciton for AddWrapToList, to create a player
void CreatePlayer(objTracker *tracker, Vector2 initPosition, float size);

// Creates a projectile that inherits from it's parent
void CreateProjectile(objTracker *tracker, objWrap *parent);

// Destructor for objWrap
void DeleteObjWrap(objWrap *wrap);

// Wrapper for objWrapper destructor that hanldes cleanup from the tracking list
void DeleteTrackedObject(objTracker *tracker, unsigned long index);

// Deinit tracker and everything in it
void DeleteTracker(objTracker *tracker);

#endif // OBJECTHANDLER_H_
