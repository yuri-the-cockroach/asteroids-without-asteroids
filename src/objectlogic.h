#ifndef OBJECTLOGIC_H_
#define OBJECTLOGIC_H_

#include "structs.h"
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <sys/types.h>

/*  ----------------------<Function definitions start
 * here>----------------------  */

// Update the object position based on the speed, acceleration, rotation etc...
void UpdateObjectPos(ObjectStruct *object);

// Rotates provided object by *object.shape.rotate* degree !!Mutating object
// inplace!!
void RotateObject(ObjectStruct *object, float rotateByDeg);

// Adjusts scale of object geometry as requested
Vector2 *ResizeShape(const Vector2 *vector, float size,
                     unsigned int arrayLength);

// Initialize ShapeStruct with default values
ShapeStruct InitShape(const Vector2 *pointArray, unsigned int arrayLength,
                      float sizeMult);

// Returns ObjectStruct ready to use with default data initialized
ObjectStruct InitObject(ShapeStruct shape, Vector2 initPosition,
                        Vector2 initSpeed, unsigned int accelSpeed,
                        float rotSpeed, float colliderMult);

// Free the ShapeStruct and cleanup
void DeleteShapeStruct(ShapeStruct *self);

// Free the ObjectStruct and cleanup
void DeleteObjectStruct(ObjectStruct *self);

#endif // OBJECTLOGIC_H_
