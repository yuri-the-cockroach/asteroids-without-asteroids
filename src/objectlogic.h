#ifndef OBJECTLOGIC_H_
#define OBJECTLOGIC_H_

#include <string.h>
#include <raylib.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <raylib.h>
#include <raymath.h>
#include <sys/types.h>

typedef struct ObjectStruct ObjectStruct;
typedef struct ShapeStruct ShapeStruct;

const Vector2 PLAYER_SHAPE_POINTS[] = {
(Vector2){   0, -100 }, // Is always treated as the fronts-most point, used to get the heading
(Vector2){  50,   50 },
(Vector2){   0,    0 },
(Vector2){ -50,   50 }
};

const Vector2 ASTEROID_SHAPE_POINTS[] = {
(Vector2){-50,   50  },
(Vector2){ 50,   50  },
(Vector2){50,  -50  },        // Is always treated as the fronts-most point, used to get the heading
(Vector2){ -50,  -50  }
};

struct ShapeStruct {
    float   sizeMult;
    unsigned int arrayLength;
    Vector2 *points;
    const Vector2 *refPoints;

    /* Function references */

    void (*TransformShapeMatrix) (ShapeStruct *self, float angle);
    void (*DeleteShapeStruct) (ShapeStruct *self);
};

struct ObjectStruct  {
    float       moveSpeed;
    float       rotateSpeed;
    float       heading;
    Vector2     position;
    Vector2     speed;
    ShapeStruct shape;

    /* Function references */

    void (*DeleteObjectStruct) (ObjectStruct *self);

};


/*  ----------------------<Function definitions start here>----------------------  */


// Update the object position based on the speed, acceleration, rotation etc...
void UpdateObjectPos(ObjectStruct *object);

// Handles acceleration of the object
void OnObjectAccelerate(ObjectStruct *object, float speed);

// Rotates provided object by *object.shape.rotate* degree !!Mutating object inplace!!
void TransformShapeMatrix(ShapeStruct *shape, float angle);

// Draws the object based on it's *object.shape.points* array
void DrawObject(ObjectStruct *object);

// Adjusts scale of object geometry as requested
Vector2* _ResizeVector2(const Vector2 *vector, float size, unsigned int arrayLength);

// Initialize ShapeStruct with default values
ShapeStruct InitShape(const Vector2 *pointArray, unsigned int arrayLength, float sizeMult);

// Returns ObjectStruct ready to use with default data initialized
ObjectStruct InitObject(ShapeStruct shape, Vector2 initPosition, Vector2 initSpeed, unsigned int accelSpeed, unsigned int rotSpeed);

// Free the ShapeStruct and cleanup
void DeleteShapeStruct(ShapeStruct *self);

// Free the ObjectStruct and cleanup
void DeleteObjectStruct(ObjectStruct *self);

#endif // OBJECTLOGIC_H_
