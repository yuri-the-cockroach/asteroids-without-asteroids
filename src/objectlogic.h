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

typedef struct object ObjectStruct;
typedef struct ShapeStruct ShapeStruct;

struct ShapeStruct {
    float   sizeMult;
    unsigned int arrayLength;
    Vector2 *points;
    const Vector2 *refPoints;

    /* Function references */

    void (*TransformShapeMatrix) (ShapeStruct *self, float angle);
};

struct object {
    float       moveSpeed;
    float       rotateSpeed;
    float       heading;
    Vector2     position;
    Vector2     speed;
    ShapeStruct shape;
} ;


/*  ----------------------<Function definitions start here>----------------------  */


//
void UpdateObjectPos(ObjectStruct *object);

// Handles acceleration of the object
void OnObjectAccelerate(ObjectStruct *object, float speed);

// Rotates provided object by *object.shape.rotate* degree !!Mutating object inplace!!
void TransformShapeMatrix(ShapeStruct *shape, float angle);

// Draws the object based on it's *object.shape.points* array
void DrawObject(ObjectStruct *object);

// Adjusts scale of object geometry as requested
Vector2* _ResizeVector2(const Vector2 *vector, float size, unsigned int arrayLength);

//
ShapeStruct InitShape(const Vector2 *pointArray, unsigned int arrayLength, float sizeMult);

// Returns ObjectStruct ready to use with default data initialized
ObjectStruct InitObject(ShapeStruct shape);
#endif // OBJECTLOGIC_H_
