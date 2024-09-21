#include "objectlogic.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>

void UpdateObjectPos(ObjectStruct *object) {
    object->position.x =
        fmod(object->position.x + object->speed.x * GetFrameTime(),
             (float)GetScreenWidth());
    if (object->position.x < 0)
        object->position.x = (float)GetScreenWidth();
    object->position.y =
        fmod(object->position.y + object->speed.y * GetFrameTime(),
             (float)GetScreenHeight());
    if (object->position.y < 0)
        object->position.y = (float)GetScreenHeight();
}

void OnObjectAccelerate(ObjectStruct *object, float speed) {
    object->speed.x = object->speed.x + object->shape.points[0].x * speed;
    object->speed.y = object->speed.y + object->shape.points[0].y * speed;
}

void TransformObjectMatrix(ShapeStruct *shape, float rotationAngle) {
    for (unsigned int current = 0; current < shape->arrayLength; current++) {
        shape->points[current].x =
            (shape->refPoints[current].x * cos(rotationAngle)) -
            (shape->refPoints[current].y * sin(rotationAngle));
        shape->points[current].y =
            (shape->refPoints[current].x * sin(rotationAngle)) +
            (shape->refPoints[current].y * cos(rotationAngle));
    }
}

void DrawObject(ObjectStruct *object) {
    int prevPoint = 0;

    for (unsigned int curPoint = 1; curPoint < object->shape.arrayLength;
         curPoint++) {
        DrawLineEx(
            (Vector2){ // Draw from x/y
                       object->shape.points[prevPoint].x + object->position.x,
                       object->shape.points[prevPoint].y + object->position.y },
            (Vector2){ // Draw to x/y
                       object->shape.points[curPoint].x + object->position.x,
                       object->shape.points[curPoint].y + object->position.y },
            2.0,  // Thikness of the line
            WHITE // Color of the line
        );
        prevPoint = curPoint;
    }

    // Draw the final line to finish the form. I'm too lazy to do this less
    // janky
    DrawLineEx(
        (Vector2){ // Draw from x/y
                   object->shape.points[0].x + object->position.x,
                   object->shape.points[0].y + object->position.y },
        (Vector2){ // Draw to x/y
                   object->shape.points[object->shape.arrayLength - 1].x +
                       object->position.x,
                   object->shape.points[object->shape.arrayLength - 1].y +
                       object->position.y },
        1.0,  // Thikness of the line
        WHITE // Color of the line
    );
}

Vector2 *_ResizeVector2(const Vector2 *vector, float size,
                        unsigned int arrayLength) {
    Vector2 *tempPShape = malloc(sizeof(Vector2) * arrayLength);

    for (unsigned int current = 0; current < arrayLength; current++) {
        tempPShape[current].x = round(vector[current].x * size);
        tempPShape[current].y = round(vector[current].y * size);
    }

    return tempPShape;
}

ShapeStruct InitShape(const Vector2 *pointArray, unsigned int arrayLength,
                      float sizeMult) {
    ShapeStruct toReturn = {
        sizeMult,
        arrayLength,
        calloc(arrayLength, sizeof(Vector2)),
        (const Vector2 *)_ResizeVector2(pointArray, sizeMult, arrayLength),
        *TransformObjectMatrix,
        *DeleteShapeStruct
    };

    memcpy((void *)toReturn.points,
           (void *)toReturn.refPoints,
           sizeof(Vector2) * arrayLength);
    return toReturn;
}

ObjectStruct InitObject(ShapeStruct shape, Vector2 initPosition,
                        Vector2 initSpeed, unsigned int accelSpeed,
                        float rotSpeed, float colliderMult) {
    return (ObjectStruct){
        accelSpeed, // acceleration speed
        rotSpeed, // Rotation speed
        0, // Starter heading
        initPosition, // Starting position of the object
        initSpeed, // Starting speed of the object
        shape,
        (Rectangle){ -50 * shape.sizeMult * colliderMult,
                    -50 * shape.sizeMult * colliderMult,
                    100 * shape.sizeMult * colliderMult,
                    100 * shape.sizeMult * colliderMult },
        0.9,
        shape.sizeMult * shape.sizeMult
    };
}

void DeleteShapeStruct(ShapeStruct *self) {
    free((void *)self->points);
    free((void *)self->refPoints);
    free(self);
}

void DeleteObjectStruct(ObjectStruct *self) {
    self->shape.DeleteShapeStruct(&self->shape);
    free(self);
}
