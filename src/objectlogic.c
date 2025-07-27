// system includes
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// local includes
#include "autils.h"
#include "collision.h"
#include "objectlogic.h"
#include "structs.h"
#include "vector-math.h"

// This will be adjusted by frametime, so it can be more than
// BOUNCEBACK_MAX_FORCE

void UpdateObjectPos(objWrap *wrap) {
    if (!wrap) return;
    pthread_mutex_lock(&wrap->mutex);
    const float PUSHBACK_STEP      = 1.f;
    const float MAX_PUSHBACK_SPEED = 200.f;
    Vector2 speed                  = wrap->objPtr->speed;
    Vector2 position               = wrap->objPtr->position;
    Vector2 colStart = { wrap->collider.collider.x, wrap->collider.collider.y };
    colStart         = VecAddVec(position, colStart);

    Vector2 colEnd        = { wrap->collider.collider.width,
                              wrap->collider.collider.height };
    colEnd                = VecAddVec(colStart, colEnd);
    Vector2 adjustedSpeed = VecMulFloat(speed, LAST_FRAME_TIME);

    if (wrap->objectType == PLAYER) speed = VecMulFloat(speed, 0.995f);

    if (wrap->objectType == PROJECTILE &&
        ((colStart.x < WORLD_POS_MIN_X && speed.x < 0) ||
         (colStart.y < WORLD_POS_MIN_Y && speed.y < 0) ||
         (colEnd.x > WORLD_POS_MAX_X && speed.x > 0) ||
         (colEnd.y > WORLD_POS_MAX_Y && speed.y > 0))) {
        wrap->request          = DELETE;
        wrap->objPtr->position = VecAddVec(position, adjustedSpeed);
        wrap->objPtr->speed    = speed;
        pthread_mutex_unlock(&wrap->mutex);
        return;
    }

    if (colStart.x <= WORLD_POS_MIN_X) {
        speed.x *= speed.x < 0 ? -1 : 1;
        speed.x += speed.x < MAX_PUSHBACK_SPEED ? PUSHBACK_STEP : 0;
    }

    if (colStart.y <= WORLD_POS_MIN_Y) {
        speed.y *= speed.y < 0 ? -1 : 1;
        speed.y += speed.y < MAX_PUSHBACK_SPEED ? PUSHBACK_STEP : 0;
    }

    if (colEnd.x >= WORLD_POS_MAX_X) {
        speed.x *= speed.x > 0 ? -1 : 1;
        speed.x += speed.x > -MAX_PUSHBACK_SPEED ? -PUSHBACK_STEP : 0;
    }

    if (colEnd.y >= WORLD_POS_MAX_Y) {
        speed.y *= speed.y > 0 ? -1 : 1;
        speed.y += speed.y > -MAX_PUSHBACK_SPEED ? -PUSHBACK_STEP : 0;
    }

    adjustedSpeed = (Vector2){ wrap->objPtr->speed.x * frameTime,
                               wrap->objPtr->speed.y * frameTime };

    wrap->objPtr->position.x = wrap->objPtr->position.x + adjustedSpeed.x;
    wrap->objPtr->position.y = wrap->objPtr->position.y + adjustedSpeed.y;
    pthread_mutex_unlock(&wrap->mutex);
}

void RotateObject(objWrap *wrap, float rotateByDeg) {
    wrap->objPtr->heading += (rotateByDeg * GetFrameTime());
    wrap->objPtr->heading =
        RollOverFloat(wrap->objPtr->heading, 0.0f, PI * 2.0f);
    float sin_heading = sinf(wrap->objPtr->heading);
    float cos_heading = cosf(wrap->objPtr->heading);
    for (unsigned int current = 0; current < wrap->objPtr->shape.arrayLength;
         current++) {
        wrap->objPtr->shape.points[current].x =
            wrap->objPtr->shape.refPoints[current].x * cos_heading -
            wrap->objPtr->shape.refPoints[current].y * sin_heading;
        wrap->objPtr->shape.points[current].y =
            wrap->objPtr->shape.refPoints[current].x * sin_heading +
            wrap->objPtr->shape.refPoints[current].y * cos_heading;
    }

    if (wrap->objectType == PLAYER && wrap->collider.isCollidable)
        UpdateCollider(wrap);
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

shape InitShape(const Vector2 *pointArray, unsigned int arrayLength,
                float sizeMult) {
    shape toReturn = { sizeMult,
                       arrayLength,
                       pointArray ? calloc(arrayLength, sizeof(Vector2)) : NULL,
                       ResizeShape(pointArray, sizeMult, arrayLength) };
    if (pointArray)
        memcpy((void *)toReturn.points,
               (void *)toReturn.refPoints,
               sizeof(Vector2) * arrayLength);
    return toReturn;
}

object InitObject(shape shape, Vector2 initPosition, Vector2 initSpeed,
                  float rotSpeed) {
    return (object){
        rotSpeed,     // Rotation speed
        0,            // Starter heading
        initPosition, // Starting position of the object
        initSpeed,    // Starting speed of the object
        shape,
    };
}

void DeleteObjectStruct(object *self) {
    free((void *)self->shape.points);
    free((void *)self->shape.refPoints);
    free((void *)self);
}
