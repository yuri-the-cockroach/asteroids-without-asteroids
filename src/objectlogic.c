#include "objectlogic.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

#define BOUNCEBACK 2

void UpdateObjectPos(ObjectStruct *object) {
    float frameTime = GetFrameTime();
    Vector2 adjustedSpeed = { object->speed.x * frameTime,
                              object->speed.y * frameTime };

    if (object->position.x + adjustedSpeed.x < BORDER_OFFSET)
        object->speed.x += BOUNCEBACK;
    if (object->position.x + adjustedSpeed.x >
        (float)GetScreenWidth() - BORDER_OFFSET)
        object->speed.x -= BOUNCEBACK;

    if (object->position.y + adjustedSpeed.y < BORDER_OFFSET)
        object->speed.y += BOUNCEBACK;
    if (object->position.y + adjustedSpeed.y >
        (float)GetScreenHeight() - BORDER_OFFSET)
        object->speed.y -= BOUNCEBACK;

    adjustedSpeed =
        (Vector2){ object->speed.x * frameTime, object->speed.y * frameTime };

    object->position.x = object->position.x + adjustedSpeed.x;
    object->position.y = object->position.y + adjustedSpeed.y;
}

void RotateObject(ObjectStruct *object, float rotateByDeg) {

    object->heading += (rotateByDeg * GetFrameTime());
    for (unsigned int current = 0; current < object->shape.arrayLength;
         current++)
    {
        object->shape.points[current].x =
            (object->shape.refPoints[current].x * cos(object->heading)) -
            (object->shape.refPoints[current].y * sin(object->heading));
        object->shape.points[current].y =
            (object->shape.refPoints[current].x * sin(object->heading)) +
            (object->shape.refPoints[current].y * cos(object->heading));
    }
}

Vector2 *ResizeShape(const Vector2 *vector, float size,
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
    ShapeStruct toReturn = { sizeMult,
                             arrayLength,
                             calloc(arrayLength, sizeof(Vector2)),
                             (const Vector2 *)ResizeShape(
                                 pointArray, sizeMult, arrayLength) };

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
    free((void *)self->shape.points);
    free((void *)self->shape.refPoints);
    free(self);
}
