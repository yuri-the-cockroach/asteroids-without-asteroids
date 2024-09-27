#include "asteroid.h"
#include "asteroidsutils.h"
#include "collision.h"
#include "structs.h"

void AsteroidSafeSpawn(ObjectTracker *tracker) {

    ObjectWrap *wrap =
        CreateAsteroid(tracker, (Vector2){ 0, 0 }, (Vector2){ 0, 0 }, 0, GetRandomFloat(1, 3));
    Vector2 newPos = (Vector2){ 0, 0};
    do {
        newPos = (Vector2) {
            GetRandomFloat(WORLD_POS_MIN_X, WORLD_POS_MAX_X),
                GetRandomFloat(WORLD_POS_MIN_Y, WORLD_POS_MAX_Y)
        };

        // This abomination will just check if the wrap closer than 200 units to the player
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
