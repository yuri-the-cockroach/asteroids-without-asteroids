// system includes
#include "raylib.h"
#include <errno.h>
#include <error.h>
#include <math.h>
#include <stdlib.h>

// local includes
#include "asteroid.h"
#include "autils.h"
#include "collision.h"
#include "logger.h"
#include "objecthandler.h"
#include "structs.h"
#include "tracingtools.c"
static Vector2 LAST_SPAWN_POS;

objWrap *AsteroidSafeSpawn(objTracker *tracker) {
    if (!tracker) {
        errno = EINVAL;
        fatal(errno, 0, "Tracker pointer provided is invalid, bailing now...");
    }
    objWrap *wrap = CreateAsteroid(tracker,
                                   LAST_SPAWN_POS,
                                   (Vector2){ 0, 0 },
                                   GetRandomf(-3, 3),
                                   GetRandomf(1, MAX_ASTEROID_SIZE));

    if (!wrap) {
        fatal(EINVAL,
              0,
              "Cannot create an asteroid, got a NULL pointer") return NULL;
    }

    bool complete = false;

    const float radius =
        MAX_ASTEROID_SIZE * ASTEROID_POINT_HIGHT + ASTEROID_HEIGHT_VARIATION;
    do {
        wrap->objPtr->position.x += radius;
        if (WORLD_POS_MAX_X < wrap->objPtr->position.x) {
            wrap->objPtr->position.x = 0;
            wrap->objPtr->position.y += radius;
        }

        if (WORLD_POS_MAX_Y < wrap->objPtr->position.y) {
            LOG(ERROR, "Failed to find a spawn point for an asteroid!");
            return NULL;
        }

        if (!FindAnyCollision(tracker, wrap)) complete = true;

    } while (!complete);
    LAST_SPAWN_POS = wrap->objPtr->position;
    if (!complete) {
        wrap->request = DELETE;
        return NULL;
    }

    const float start_speed = 4000.f;
    Vector2 astPos          = wrap->objPtr->position;
    float gamma             = atan2f(astPos.y, astPos.x);
    if (!tracker->playerPtr) gamma = GetRandomf(0, PI * 2);
    wrap->objPtr->speed =
        (Vector2){ cosf(gamma) * (GetRandomf(-start_speed, start_speed)),
                   sinf(gamma) * (GetRandomf(-start_speed, start_speed)) };
    return wrap;
}

// This function takes care of
// * Creating and instanciating the object
// * Adding it into the tracked list
// * Giving values to anything that needs them
// Returnes a ready to use object, that needs no tweaks to work.
objWrap *CreateAsteroid(objTracker *tracker, Vector2 initPosition,
                        Vector2 initSpeed, float constRotationSpeed,
                        float size) {

    objWrap *asteroid      = malloc(sizeof(objWrap));
    asteroid[0]            = InitWrap();
    asteroid[0].objectType = ASTEROID;
    if (AddWrapToList(tracker, asteroid)) {
        errno = 0;
        free(asteroid); // TODO: Make a propper logger and call it when
                        // something goes wrong
        return NULL;
    }

    object *objPtr      = malloc(sizeof(object));
    Vector2 *tempPoints = GenerateAsteroidShape();
    objPtr[0] = InitObject(InitShape(tempPoints, ASTEROID_CORNERS_COUNT, size),
                           initPosition,
                           initSpeed,
                           constRotationSpeed);
    free(tempPoints);

    asteroid->objectType        = ASTEROID;
    asteroid->request           = CREATE;
    asteroid->updatePosition    = true;
    asteroid->draw              = true;
    asteroid->isRotatableByGame = true;
    asteroid->objPtr            = objPtr;
    asteroid->collider =
        InitCollider(0.85f * asteroid->objPtr->shape.sizeMult, *Bounce);
    asteroid->livesLeft = 2;
    UpdateCollider(asteroid);
    return asteroid;
}

Vector2 *GenerateAsteroidShape(void) {
    Vector2 *CornerList =
        (Vector2 *)calloc(ASTEROID_CORNERS_COUNT, sizeof(Vector2));
    for (unsigned long i = 0; i < ASTEROID_CORNERS_COUNT; i++) {
        CornerList[i] = (Vector2){
            (ASTEROID_POINT_HIGHT + GetRandomf(-ASTEROID_HEIGHT_VARIATION,
                                               ASTEROID_HEIGHT_VARIATION)) *
                sinf(PI / (ASTEROID_CORNERS_COUNT / 2.0f) * (float)i),
            (ASTEROID_POINT_HIGHT + GetRandomf(-ASTEROID_HEIGHT_VARIATION,
                                               ASTEROID_HEIGHT_VARIATION)) *
                cosf(PI / (ASTEROID_CORNERS_COUNT / 2.0f) * (float)i)
        };
    }
    return CornerList;
}

int Separate(objTracker *tracker, objWrap *parent) {

    const float separationAddsSpeed = 50;

    if (parent->objectType != ASTEROID) {
        LOG(ERROR,
            "%s",
            "Asteroid separate function called on non-asteroid object. How?");
        return -1;
    }

    if (parent->livesLeft == 0) {
        parent->request = DELETE;
        return 0;
    }

    objWrap *asteroidLeft = CreateAsteroid(
        tracker,
        (Vector2){ parent->objPtr->position.x, parent->objPtr->position.y },
        (Vector2){ parent->objPtr->speed.x, parent->objPtr->speed.y },
        -parent->objPtr->rotateSpeed,
        1);
    objWrap *asteroidRight = CreateAsteroid(
        tracker,
        (Vector2){ parent->objPtr->position.x, parent->objPtr->position.y },
        (Vector2){ parent->objPtr->speed.x, parent->objPtr->speed.y },
        parent->objPtr->rotateSpeed,
        1);

    if (!asteroidLeft || !asteroidRight) {
        LOG(WARNING,
            "%s",
            "Create asteroid returned null. Gonna just remove the parent");
        parent->request = DELETE;

        return -1;
    }

    asteroidLeft->livesLeft  = parent->livesLeft;
    asteroidRight->livesLeft = parent->livesLeft;

    asteroidLeft->objPtr->heading  = parent->objPtr->heading;
    asteroidRight->objPtr->heading = parent->objPtr->heading;

    asteroidLeft->objPtr->position.x -=
        sinf(parent->objPtr->heading) *
        (parent->collider.collider.x + parent->collider.collider.width);
    asteroidLeft->objPtr->position.y -=
        cosf(parent->objPtr->heading) *
        (parent->collider.collider.y + parent->collider.collider.height);

    asteroidRight->objPtr->position.x +=
        sinf(parent->objPtr->heading) *
        (parent->collider.collider.x + parent->collider.collider.width);
    asteroidRight->objPtr->position.y +=
        cosf(parent->objPtr->heading) *
        (parent->collider.collider.y + parent->collider.collider.height);

    asteroidLeft->objPtr->speed.x -=
        separationAddsSpeed * sinf(parent->objPtr->heading);
    asteroidLeft->objPtr->speed.y -=
        separationAddsSpeed * cosf(parent->objPtr->heading);

    asteroidRight->objPtr->speed.x +=
        separationAddsSpeed * sinf(parent->objPtr->heading);
    asteroidRight->objPtr->speed.y +=
        separationAddsSpeed * cosf(parent->objPtr->heading);

    asteroidLeft->collider.mass /= 2;
    asteroidRight->collider.mass /= 2;

    asteroidLeft->objPtr->shape.arrayLength =
        parent->objPtr->shape.arrayLength / 2;
    asteroidRight->objPtr->shape.arrayLength =
        parent->objPtr->shape.arrayLength / 2;

    for (unsigned long i = 0; i < parent->objPtr->shape.arrayLength / 2; i++) {
        asteroidRight->objPtr->shape.refPoints[i] =
            parent->objPtr->shape.refPoints[i];
        asteroidLeft->objPtr->shape.refPoints[i] =
            parent->objPtr->shape
                .refPoints[parent->objPtr->shape.arrayLength / 2 + i];
    }

    LOG(DEBUG,
        "\nAsteroidLeft speed x: %f\nAsteroidRight speed x %f",
        asteroidLeft->objPtr->speed.x,
        asteroidRight->objPtr->speed.x);

    parent->request = DELETE;
    return 0;
}
