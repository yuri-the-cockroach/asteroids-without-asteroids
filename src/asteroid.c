#include "asteroid.h"
#include "asteroidsutils.h"
#include "collision.h"
#include "objecthandler.h"
#include "structs.h"
#include "logger.h"
#include <math.h>

void AsteroidSafeSpawn(ObjectTracker *tracker) {

    ObjectWrap *wrap = CreateAsteroid(
        tracker, (Vector2){ 0, 0 }, (Vector2){ 0, 0 }, 0, GetRandomFloat(1, 3));
    Vector2 newPos = (Vector2){ 0, 0 };
    do {
        newPos = (Vector2){ GetRandomFloat(WORLD_POS_MIN_X, WORLD_POS_MAX_X),
                            GetRandomFloat(WORLD_POS_MIN_Y, WORLD_POS_MAX_Y) };

        // This abomination will just check if the wrap closer than 200 units to
        // the player
        if (tracker->playerPtr &&
            fabsf(fabsf(tracker->playerPtr->objPtr->position.x) -
                  fabsf(newPos.x)) < 200 &&
            fabsf(fabsf(tracker->playerPtr->objPtr->position.y) -
                  fabsf(newPos.y)) < 200)
            continue;

        wrap->objPtr->position = newPos;
    } while (FindAnyCollision(tracker, wrap));
}

ObjectWrap *CreateAsteroid(ObjectTracker *tracker, Vector2 initPosition,
                           Vector2 initSpeed, float constRotationSpeed,
                           float size) {

    ObjectWrap *asteroid = malloc(sizeof(ObjectWrap));
    asteroid[0] = InitWrap();
    if (AddWrapToList(tracker, asteroid)) {
        errno = 0;
        free(asteroid); // TODO: Make a propper logger and call it when
                        // something goes wrong
        return NULL;
    }

    ObjectStruct *objPtr = malloc(sizeof(ObjectStruct));
    objPtr[0] = InitObject(
        InitShape(GenerateAsteroidShape(), ASTEROID_CORNERS_COUNT, size),
        initPosition,
        initSpeed,
        constRotationSpeed);

    asteroid->objectType = ASTEROID;
    asteroid->request = UPDATE;
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
    Vector2 *CornerList = calloc(ASTEROID_CORNERS_COUNT, sizeof(Vector2));
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

    float changeDirectionBy = 10;

    if (parent->objectType != ASTEROID) {
        LOG(ERROR,
            "%s",
            "Asteroid separate function called on non-asteroid object. How?");
        return -1;
    }

    if ( parent->livesLeft == 0 ) {
        parent->request = DELETE;
        return 0;
    }

    ObjectWrap *asteroidLeft = CreateAsteroid(
        tracker,
        (Vector2){ parent->objPtr->position.x, parent->objPtr->position.y },
        (Vector2){ parent->objPtr->speed.x, parent->objPtr->speed.y   },
        parent->objPtr->rotateSpeed,
        1);
    ObjectWrap *asteroidRight = CreateAsteroid(
        tracker,
        (Vector2){ parent->objPtr->position.x, parent->objPtr->position.y },
        (Vector2){ parent->objPtr->speed.x, parent->objPtr->speed.y   },
        -parent->objPtr->rotateSpeed,
        1);


    if ( !asteroidLeft || !asteroidRight ) {
        LOG(ERROR, "%s", "Create asteroid returned null");
        return -1;
    }

    asteroidLeft->livesLeft = parent->livesLeft;
    asteroidRight->livesLeft = parent->livesLeft;

    asteroidLeft->objPtr->heading = parent->objPtr->heading;
    asteroidRight->objPtr->heading = parent->objPtr->heading;

    asteroidLeft->objPtr->position.x += 60;
    asteroidRight->objPtr->position.x -= 60;

    asteroidLeft->objPtr->speed.x = parent->objPtr->speed.x * sinf(tracker->playerPtr->objPtr->heading);
    asteroidRight->objPtr->speed.y = parent->objPtr->speed.y * cosf(tracker->playerPtr->objPtr->heading);

    asteroidLeft->objPtr->speed.x = parent->objPtr->speed.x * sinf(tracker->playerPtr->objPtr->heading);
    asteroidRight->objPtr->speed.y = parent->objPtr->speed.y * cosf(tracker->playerPtr->objPtr->heading);

    asteroidLeft->collider.mass /= 2;
    asteroidRight->collider.mass /= 2;

    asteroidLeft->objPtr->shape.arrayLength = parent->objPtr->shape.arrayLength / 2;
    asteroidRight->objPtr->shape.arrayLength = parent->objPtr->shape.arrayLength / 2;


    for ( unsigned long i = 0; i < parent->objPtr->shape.arrayLength / 2; i++ ) {
        asteroidRight->objPtr->shape.refPoints[i] = parent->objPtr->shape.refPoints[i];
        asteroidLeft->objPtr->shape.refPoints[i] = parent->objPtr->shape.refPoints[parent->objPtr->shape.arrayLength / 2 + i];

    }



    LOG(DEBUG, "\nAsteroidLeft speed x: %f\nAsteroidRight speed x %f", asteroidLeft->objPtr->speed.x, asteroidRight->objPtr->speed.x);

    parent->request = DELETE;
    return 0;
}
