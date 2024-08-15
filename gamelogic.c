#include "headers/gamelogic.h"
#include <raymath.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>


// This one is responsible for moving stuff
// It will go through the *PhysObjTracker.physObjArray*
// and update the position of each based on the forces it is affected by
int UpdatePhysObjects(BasePhysStruct *objArray) {
    return 0;
}

// This one will create a phys object
// init all the values and return it
BasePhysStruct InitPhysObj(unsigned int index) {
    return (BasePhysStruct) {
        index,              // Index
        0,                  // Heading
        (Vector2) { 0, 0 }, // Position
        (Vector2) { 0, 0 }, // speed
    };
}

// Initializes a PhysObjTracker and returns it prefilled with default data
// This function is a possible cancelation point.
// It will exit if errno is not set to 0
// For this reason, program should initialize errno and set it to 0 on the start.
PhysObjTracker InitTracker() {
    BasePhysStruct *tempPointer = (BasePhysStruct*) malloc(sizeof(BasePhysStruct) * MAX_PHYS_OBJ);
    if (errno != 0) err(errno, "Cought an error in InitTracker function\n Exiting now...\n");
    return (PhysObjTracker) { 0, tempPointer };
}

// returns 0 if successful
// This one will use the InitPhsyObj function to creat the object
// and register it with the tracker
int NewPhysObj(PhysObjTracker *tracker) {
    tracker->physObjArray[tracker->physObjCount] = InitPhysObj(tracker->physObjCount);
    tracker->physObjCount++;
}

// This one will get rid of the object
// Make sure that there is no memory leaks
// And remove the object from the tracking array
int DelPhysObj(PhysObjTracker *tracket, BasePhysStruct *obj);

// Returns player rotated by *player.shape.rotate* degre without mutating it
BasePhysStruct TransformPhysObjMatrix(BasePhysStruct *obj, float rotationAngle);

// Rotates provided player by *player.shape.rotate* degree !!Mutating player inplace!!
// Returns 0 if successful, any other return code is reserved as an error code
int MutTransformPhysObjMatrix(BasePhysStruct *obj, float rotationAngle);
