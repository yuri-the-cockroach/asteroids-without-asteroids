// system includes
#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <errno.h>

// local includes
#include "asteroid.h"
#include "autils.h"
#include "collision.h"
#include "logger.h"
#include "objecthandler.h"
#include "structs.h"

ObjectWrap *AsteroidSafeSpawn(ObjectTracker *tracker) {

    ObjectWrap *wrap = CreateAsteroid(
        tracker,
        (Vector2){ 0, 0 },
        (Vector2){
        GetRandomFloat(-100, 100),
        GetRandomFloat(-100, 100)
    },
        GetRandomFloat(-3, 3),
        GetRandomFloat(1, 3));

    if ( !wrap ) {
        LOG(WARNING, "%s", "Cannot create an asteroid, got a NULL pointer");
        return NULL;
    }

    Vector2 playerPos = tracker->playerPtr ? tracker->playerPtr->objPtr->position : (Vector2){0, 0};
    Vector2 newPos = (Vector2){ 0, 0 };
    do {
        newPos = (Vector2){ GetRandomFloat(WORLD_POS_MIN_X, WORLD_POS_MAX_X),
                            GetRandomFloat(WORLD_POS_MIN_Y, WORLD_POS_MAX_Y)};
        // This abomination will just check if the wrap closer than 200u or further than 1000u to
        // the player
        if (tracker->playerPtr && (
                fabsf(playerPos.x - newPos.x) < 300
                || fabsf(playerPos.x - newPos.x) > 4000
                || fabsf(playerPos.y - newPos.y) < 300
                || fabsf(playerPos.y - newPos.y) > 4000
            )
        ) continue;
        wrap->objPtr->position = newPos;
    } while (FindAnyCollision(tracker, wrap));

    float gamma = atan2f(playerPos.x - newPos.x, playerPos.y - newPos.y);
    wrap->objPtr->speed = (Vector2){
        GetRandomFloat(-20, 20) + sinf(gamma) * ( GetRandomFloat(20, 100) ),
        GetRandomFloat(-20, 20) + cosf(gamma) * ( GetRandomFloat(20, 100) )
    };
    return wrap;
}

ObjectWrap *CreateAsteroid(ObjectTracker *tracker, Vector2 initPosition,
                           Vector2 initSpeed, float constRotationSpeed,
                           float size) {

    ObjectWrap *asteroid = malloc(sizeof(ObjectWrap));
    asteroid[0] = InitWrap();
    asteroid[0].objectType = ASTEROID;
    if (AddWrapToList(tracker, asteroid)) {
        errno = 0;
        free(asteroid); // TODO: Make a propper logger and call it when
                        // something goes wrong
        return NULL;
    }

    ObjectStruct *objPtr = malloc(sizeof(ObjectStruct));
    Vector2 *tempPoints = GenerateAsteroidShape();
    objPtr[0] = InitObject(
        InitShape(tempPoints, ASTEROID_CORNERS_COUNT, size),
        initPosition,
        initSpeed,
        constRotationSpeed);
    free(tempPoints);

    asteroid->objectType = ASTEROID;
    asteroid->request = CREATE;
    asteroid->updatePosition = true;
    asteroid->draw = true;
    asteroid->isRotatableByGame = true;
    asteroid->objPtr = objPtr;
    asteroid->collider =
        InitCollider(0.85f * asteroid->objPtr->shape.sizeMult, *Bounce);
    asteroid->livesLeft = 2;
    return asteroid;
}

Vector2 *GenerateAsteroidShape(void) {
    Vector2 *CornerList = (Vector2 *)calloc(ASTEROID_CORNERS_COUNT, sizeof(Vector2));
    for (unsigned long i = 0; i < ASTEROID_CORNERS_COUNT; i++) {
        CornerList[i] = (Vector2){
            (50 + GetRandomFloat(-ASTEROID_HEIGHT_VARIATION,
                                 ASTEROID_HEIGHT_VARIATION)) *
                sinf(PI / (ASTEROID_CORNERS_COUNT / 2.0f) * (float)i),
            (50 + GetRandomFloat(-ASTEROID_HEIGHT_VARIATION,
                                 ASTEROID_HEIGHT_VARIATION)) *
                cosf(PI / (ASTEROID_CORNERS_COUNT / 2.0f) * (float)i)
        };
    }
    return CornerList;
}

int Separate(ObjectTracker *tracker, ObjectWrap *parent) {

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

    ObjectWrap *asteroidLeft = CreateAsteroid(
        tracker,
        (Vector2){ parent->objPtr->position.x, parent->objPtr->position.y },
        (Vector2){ parent->objPtr->speed.x, parent->objPtr->speed.y },
        -parent->objPtr->rotateSpeed,
        1);
    ObjectWrap *asteroidRight = CreateAsteroid(
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

    asteroidLeft->livesLeft = parent->livesLeft;
    asteroidRight->livesLeft = parent->livesLeft;

    asteroidLeft->objPtr->heading = parent->objPtr->heading;
    asteroidRight->objPtr->heading = parent->objPtr->heading;

    asteroidLeft->objPtr->position.x -= sinf(parent->objPtr->heading) * (parent->collider.collider.x + parent->collider.collider.width);
    asteroidLeft->objPtr->position.y -= cosf(parent->objPtr->heading) * (parent->collider.collider.y + parent->collider.collider.height);

    asteroidRight->objPtr->position.x += sinf(parent->objPtr->heading) * (parent->collider.collider.x + parent->collider.collider.width);
    asteroidRight->objPtr->position.y += cosf(parent->objPtr->heading) * (parent->collider.collider.y + parent->collider.collider.height);

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
