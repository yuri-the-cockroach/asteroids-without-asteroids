#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <raylib.h>


/* -------------------- enums --------------------  */

enum request { IGNORE = 1, UPDATE = 0, DELETE = -1 };

/* -------------------- typedefs --------------------  */

typedef struct tracker ObjectTracker;
typedef struct ObjectWrap ObjectWrap;
typedef struct ObjectStruct ObjectStruct;
typedef struct ShapeStruct ShapeStruct;

/* -------------------- constants --------------------  */

#define MAX_OBJECT_COUNT 1024
#define PLAYER_ROTATION_SPEED 5
#define PLAYER_MOVE_SPEED 5

#define BASE_ACCELL 5
#define BASE_ROTATE 5

#define DEBUG true
#define BORDER_OFFSET 40

unsigned int SCREEN_WIDTH = 1600;
unsigned int SCREEN_HEIGHT = 900;

const Vector2 PLAYER_SHAPE_POINTS[] = {
    (Vector2){ 0,   -50 }, // Is always treated as the fronts-most point, used to get the heading
    (Vector2){ 50,  50   },
    (Vector2){ 0,   20    },
    (Vector2){ -50, 50   }
};

const Vector2 ASTEROID_SHAPE_POINTS[] = {
    (Vector2){ -50,  50  },
    (Vector2){  50,  50  },
    (Vector2){  50, -50 }, // Is always treated as the fronts-most point, used to get the heading
    (Vector2){ -50, -50 }
};

/* -------------------- structs --------------------  */

struct ShapeStruct {
    float sizeMult;
    unsigned int arrayLength;
    Vector2 *points;
    const Vector2 *refPoints;
    /* Function references */
};

struct ObjectStruct {
    float moveSpeed;
    float rotateSpeed;
    float heading;
    Vector2 position;
    Vector2 speed;
    ShapeStruct shape;
    Rectangle collider;
    float friction;
    float mass;
};


struct ObjectWrap {
    unsigned long index; // Index of this object in tracker->objList
    enum request request; // What to do with this object on the next iteration
    bool player; // True if the object is the player. If so, keyboard handler
                 // will be called on it
    bool updatePosition; // True if object's position needs to be updated
    bool draw;           // True if object needs to be drawn
    bool collider;// True if object's collision is enabled
    bool isRotatableByGame;       // Constant rotation speed, if needed. Ignored if 0
    ObjectStruct *objPtr; // Pointer to the actuall object that is governed
    bool ignoreNext;

    void (*ActionOnCollision)(ObjectWrap *wrap);
};

struct tracker {
    bool hasPlayer;
    ObjectWrap **objList;
    unsigned long objListLen;
    ObjectStruct **drawList; // List of objects that need to be drawn
    unsigned long drawListLen;
};
/* -------------------- end --------------------  */
#endif // STRUCTS_H_
