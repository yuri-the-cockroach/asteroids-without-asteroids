#ifndef OBJECTHANDLER_H_
#define OBJECTHANDLER_H_

#define MAX_OBJECT_COUNT 2048

#include "gamelogic.h"
#include "objectlogic.h"
#include <errno.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tracker ObjectTracker;
typedef struct ObjectWrap ObjectWrap;

enum request { IGNORE = 1, UPDATE = 0, DELETE = -1 };

struct ObjectWrap {

    ObjectTracker *parent; // Pointer to the tracker, handalling this object
    unsigned long index;   // Index of this object in self->parent->objList

    enum request request; // What to do with this object on the next iteration

    bool player; // True if the object is the player. If so, keyboard handler
                 // will be called on it
    bool updatePosition;    // True if object's position needs to be updated
    bool draw;              // True if object needs to be drawn
    bool collider;          // True if object's collision is enabled
    int cRotSpeed; // Constant rotation speed, if needed. Ignored if 0

    ObjectStruct *objPtr; // Pointer to the actuall object that is governed

    /* ------<Function pointers>------ */

    void (*UpdateObj)(ObjectWrap *self); // Pointer to the function that handles
                                         // all the updating
    void (*WrapDestructor)(
        ObjectWrap *self); // Pointer to the "delete and cleanup" function
};

struct tracker {
    bool hasPlayer;
    ObjectWrap **objList;
    unsigned long objListLen;
    ObjectStruct **drawList; // List of objects that need to be drawn
    unsigned long drawListLen;

    /* ------<Function pointers>------ */

    void (*DeleteTrackedObject)(ObjectTracker *self, ObjectWrap *wrap);
};

void RunActionList(ObjectTracker *self);

/* Called on a single ObjectWrap* and goes through the update checklist */
void UpdateObj(ObjectWrap *self);

/* Takes care of safely deleting the object and cleaning up after it */
void DeleteObjWrap(ObjectWrap *self);

/* Adds an object to the list for drawing on the next frame. List gets emptied
 * after the frame is drawn */
void AddToDrawList(ObjectTracker *self, ObjectStruct *obj);

/* Calls DrawObject on each object in self->drawList
 * nulls it's position in the list after the object is draw
 * and finnaly sets self->drawListLen to 0 when everything is done */
void DrawAllFromDrawList(ObjectTracker *self);

/* Initializes the tracker and returns it */
ObjectTracker InitTracker();

void AddWrapToList(ObjectTracker *self, ObjectWrap *wrap);
void CreatePlayer(ObjectTracker *self, Vector2 initPosition,
                  unsigned int accelSpeed, unsigned int rotSpeed);
void CreateAsteroid(ObjectTracker *self, Vector2 initPosition,
                    Vector2 initSpeed, int constRotationSpeed);

void DeleteObjWrap(ObjectWrap *self);
void DeleteTrackedObject(ObjectTracker *self, ObjectWrap *wrap);
#endif // OBJECTHANDLER_H_
