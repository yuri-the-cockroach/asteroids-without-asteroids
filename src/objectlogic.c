// system includes
#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

// local includes
#include "objectlogic.h"
#include "autils.h"
#include "collision.h"
#include "structs.h"

// This will be adjusted by frametime, so it can be more than
// BOUNCEBACK_MAX_FORCE

void UpdateObjectPos(ObjectWrap *wrap) {
    float frameTime = GetFrameTime();
    Vector2 adjustedSpeed = { wrap->objPtr->speed.x * frameTime,
                              wrap->objPtr->speed.y * frameTime };

    if (wrap->objPtr->position.x + wrap->collider.collider.x < WORLD_POS_MIN_X && wrap->objPtr->speed.x < 0)
        wrap->objPtr->speed.x *= -1;

    if (wrap->objPtr->position.y + wrap->collider.collider.y < WORLD_POS_MIN_Y && wrap->objPtr->speed.y < 0)
        wrap->objPtr->speed.y *= -1;

    if (wrap->objPtr->position.x > WORLD_POS_MAX_X - (wrap->collider.collider.x + wrap->collider.collider.height) && wrap->objPtr->speed.x > 0)
        wrap->objPtr->speed.x *= -1;

    if (wrap->objPtr->position.y > WORLD_POS_MAX_Y - (wrap->collider.collider.y + wrap->collider.collider.height) && wrap->objPtr->speed.y > 0)
        wrap->objPtr->speed.y *= -1;

    adjustedSpeed =
        (Vector2){ wrap->objPtr->speed.x * frameTime, wrap->objPtr->speed.y * frameTime };

    wrap->objPtr->position.x = wrap->objPtr->position.x + adjustedSpeed.x;
    wrap->objPtr->position.y = wrap->objPtr->position.y + adjustedSpeed.y;
}

void RotateObject(ObjectWrap *wrap, float rotateByDeg) {

    wrap->objPtr->heading += (rotateByDeg * GetFrameTime());
    wrap->objPtr->heading = RollOverFloat(wrap->objPtr->heading, 0.0f, PI * 2.0f);
    for (unsigned int current = 0; current < wrap->objPtr->shape.arrayLength;
         current++)
    {
        wrap->objPtr->shape.points[current].x =
            (float)((wrap->objPtr->shape.refPoints[current].x *
                     cosf(wrap->objPtr->heading)) -
                    (wrap->objPtr->shape.refPoints[current].y *
                     sinf(wrap->objPtr->heading)));
        wrap->objPtr->shape.points[current].y =
            (wrap->objPtr->shape.refPoints[current].x * sinf(wrap->objPtr->heading)) +
            (wrap->objPtr->shape.refPoints[current].y * cosf(wrap->objPtr->heading));
    }

    if ( wrap->collider.isCollidable ) UpdateCollider(wrap);
}

Vector2 *ResizeShape(const Vector2 *vector, float size,
                     unsigned int arrayLength) {
    Vector2 *tempPShape = malloc(sizeof(Vector2) * arrayLength);

    for (unsigned int current = 0; current < arrayLength; current++) {
        tempPShape[current].x = roundf(vector[current].x * size);
        tempPShape[current].y = roundf(vector[current].y * size);
    }

    return tempPShape;
}

ShapeStruct InitShape(const Vector2 *pointArray, unsigned int arrayLength,
                      float sizeMult) {
    ShapeStruct toReturn = { sizeMult,
                             arrayLength,
                             pointArray ? calloc(arrayLength, sizeof(Vector2)) : NULL,
                             ResizeShape(pointArray, sizeMult, arrayLength) };
    if ( pointArray )
        memcpy((void *)toReturn.points,
            (void *)toReturn.refPoints,
            sizeof(Vector2) * arrayLength);
    return toReturn;
}


ObjectStruct InitObject(ShapeStruct shape, Vector2 initPosition,
                        Vector2 initSpeed, float rotSpeed) {
    return (ObjectStruct){
        rotSpeed, // Rotation speed
        0, // Starter heading
        initPosition, // Starting position of the object
        initSpeed, // Starting speed of the object
        shape,
    };
}

void DeleteObjectStruct(ObjectStruct *self) {
    free((void *)self->shape.points);
    free((void *)self->shape.refPoints);
    free((void *)self);
}
