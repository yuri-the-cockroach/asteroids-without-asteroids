#ifndef OBJECTLOGIC_H_
#define OBJECTLOGIC_H_

#include "structs.h"

// Update the object position based on the speed, acceleration, rotation etc...
void UpdateObjectPos(objWrap *wrap);

// Rotates provided object by *object.shape.rotate* degree
// !!Mutating object inplace!!
void RotateObject(objWrap *wrap, float rotateByDeg);

// Adjusts scale of object geometry as requested
Vector2 *ResizeShape(const Vector2 *vector, float size,
                     unsigned int arrayLength);

// Initialize ShapeStruct with default values
shape InitShape(const Vector2 *pointArray, unsigned int arrayLength,
                      float sizeMult);



// Returns ObjectStruct ready to use with default data initialized
object InitObject(shape shape, Vector2 initPosition,
                        Vector2 initSpeed,
                        float rotSpeed);

// Free the ShapeStruct and cleanup
void DeleteShapeStruct(shape *self);

// Free the ObjectStruct and cleanup
void DeleteObjectStruct(object *self);

#endif // OBJECTLOGIC_H_
