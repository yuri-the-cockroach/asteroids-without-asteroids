#ifndef GAMELOGIC_H_
#define GAMELOGIC_H_
#include <raymath.h>

const unsigned int MAX_PHYS_OBJ = 2048; // Hopefuly this is going to be enough, although 2048 seems kinda low...
const unsigned int BASE_ACCELL  = 5; // Base accelleration speed for the player
const unsigned int BASE_ROTATE  = 5; // Base rotation speed for the player


struct BaseShape {
    float         scale;        // To increase/decrease size of the object without compleatly remapping it
    unsigned int  pointsCount;  // Number of points that are stored in the *pointsArray*
    Vector2      *pointsArray;  // Array fo the points that describe the geometrly of the object
} typedef BaseShape;

struct BasePhysStruct {
    unsigned int indexInArray;  // Index in the *PhysObjTracker* that this object is indexed by
    float        heading;       // Heading of the object (not sure if this one is going to be used yet) TODO: Figure out, if I need this
    Vector2      position;      // Position of the object in the world
    Vector2      speed;         // Speed at which the object is moving. Used to update the *BasePhysStruct.position* in the *UpdatePhysObjects* function
} typedef BasePhysStruct;

// This one will keep track of all the physics objects and (hopefully) keep me from leaking memory on them
struct PhysObjTracker {
    unsigned int    physObjCount;
    BasePhysStruct *physObjArray; // Hopefully this is going to be enough
} typedef PhysObjTracker;

/*
 * --------------------------------------- Functions -----------------------------------------
 */



// This one is responsible for moving stuff
// It will go through the *PhysObjTracker.physObjArray*
// and update the position of each based on the forces it is affected by
int UpdatePhysObjects(BasePhysStruct *objArray);

// This one will create a phys object
// init all the values and return it
BasePhysStruct InitPhysObj();

// Initializes a PhysObjTracker and returns it prefilled with default data
PhysObjTracker InitTracker();

// returns 0 if successful
// This one will use the InitPhsyObj function to creat the object
// and register it with the tracker
int NewPhysObj(PhysObjTracker *tracket);

// This one will get rid of the object
// Make sure that there is no memory leaks
// And remove the object from the tracking array
int DelPhysObj(PhysObjTracker *tracket, BasePhysStruct *obj);

// Returns player rotated by *player.shape.rotate* degre without mutating it
BasePhysStruct TransformPhysObjMatrix(BasePhysStruct *obj, float rotationAngle);

// Rotates provided player by *player.shape.rotate* degree !!Mutating player inplace!!
// Returns 0 if successful, any other return code is reserved as an error code
int MutTransformPhysObjMatrix(BasePhysStruct *obj, float rotationAngle);

#endif // GAMELOGIC_H_
