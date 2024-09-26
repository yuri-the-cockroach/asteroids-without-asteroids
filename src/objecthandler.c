#include "objecthandler.h"
#include "structs.h"

// Returns a list of objects that need to be drawn
void RunActionList(ObjectTracker *tracker) {

    // Run through all the tracked objects
    for (unsigned long i = 0; i < tracker->objListLen; i++) {

        // Check if current object is NULLed (it really shouldn't be)
        if (tracker->objList[i] == 0)
            continue;
        // If it is somehow NULL, then cleanup:
        // Check if there's any elements ahead at all. If not just decrement
        // the length of the list.

        ObjectWrap *current = tracker->objList[i];
        switch (current->request) {
        case DELETE: // Delete element from the list
            DeleteTrackedObject(tracker, i);
            continue;

        case UPDATE: // Call the updater function of the element
            UpdateObj(tracker, current);
            break;

        case IGNORE:
            continue;
        }
    }
    CleanupMemory(tracker);
    CleanupLists(tracker);
}

void UpdateObj(ObjectTracker *tracker, ObjectWrap *wrap) {

    if ( wrap->collider.isCollidable)
        FindCollisions(tracker, wrap);

    if (wrap->isRotatableByGame) {
        RotateObject(wrap->objPtr, (wrap->objPtr->rotateSpeed));
    }

    if (wrap->updatePosition)
        UpdateObjectPos(wrap);

}

ObjectTracker *InitTracker(void) {
    ObjectTracker *tracker = calloc(1, sizeof(ObjectTracker));
    tracker[0] = (ObjectTracker){
        NULL,
        { 0 },
        (ObjectWrap **)calloc(MAX_OBJECT_COUNT, sizeof(ObjectWrap *)),
        0
    };
    return tracker;
}

ObjectWrap InitWrap(void) {
    return (ObjectWrap){
        NOTYPE,
        IGNORE,
        false,
        false,
        false,
        false,
        {
        false,
        { 0, 0, 0, 0 },
        0,
        NULL,
        0,
        NULL }
    };
}

int AddWrapToList(ObjectTracker *tracker, ObjectWrap *wrap) {

    if (!wrap) {
        errno = EFAULT;
        LOG(ERROR, "Got null pointer instead of wrap. errno: %d", tracker->objListLen, errno);
        return errno;
    }

    if (tracker->objListLen >= MAX_OBJECT_COUNT) {
        errno = ECHRNG;
        LOG(ERROR, "Too many objects in tracker->objListLen is at %ld. errno: %d", tracker->objListLen, errno);
        return errno;
    }


    if ( wrap->objectType == PLAYER  ) {
        if ( tracker->playerPtr ) {
            errno = EADDRINUSE;
            LOG(ERROR, "Player is already present in the list. errno: %d", errno);
            return errno;
        }
        tracker->playerPtr = wrap;
    }


    tracker->objList[tracker->objListLen] = wrap;
    tracker->objListLen++;
    return 0;
}

// Pass the default speed, default rotation speed, default position
void CreatePlayer(ObjectTracker *tracker, Vector2 initPosition, float size) {

    ObjectWrap *player = malloc(sizeof(ObjectWrap));
    player[0] = InitWrap();
    player->objectType = PLAYER;

    if (AddWrapToList(tracker, player)) {
        free(player);
        return;
    }

    ObjectStruct *objPtr = malloc(sizeof(ObjectStruct));
    objPtr[0] =
        InitObject(InitShape(PLAYER_SHAPE_POINTS,
                             sizeof(PLAYER_SHAPE_POINTS) / sizeof(Vector2),
                             size),
                   initPosition,
                   (Vector2){ 0, 0 },
                   0);

    player->request = UPDATE;
    player->isRotatableByGame = false;
    player->updatePosition = true;
    player->draw = true;
    player->objPtr = objPtr;

    player->collider = InitCollider(player->objPtr->shape.sizeMult, *Bounce);
    player->collider.mass = 1;

    // Camera stuff
    tracker->playerCamera.target = (Vector2){ tracker->objList[0]->objPtr->position.x + 20.0f,
                               tracker->objList[0]->objPtr->position.y + 20.0f };
    tracker->playerCamera.offset =
        (Vector2){ (float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f };

    tracker->playerCamera.rotation = 0.0f;
    tracker->playerCamera.zoom = 1.0f;


}

void CreateAsteroid(ObjectTracker *tracker, Vector2 initPosition,
                    Vector2 initSpeed, float constRotationSpeed, float size) {

    ObjectWrap *asteroid = malloc(sizeof(ObjectWrap));
    asteroid[0] = InitWrap();
    if (AddWrapToList(tracker, asteroid)) {
        errno = 0;
        free(asteroid); // TODO: Make a propper logger and call it when
                        // something goes wrong
        return;
    }

    ObjectStruct *objPtr = malloc(sizeof(ObjectStruct));
    objPtr[0] =
        InitObject(InitShape(ASTEROID_SHAPE_POINTS,
                             sizeof(ASTEROID_SHAPE_POINTS) / sizeof(Vector2),
                             size),
                   initPosition,
                   initSpeed,
                   constRotationSpeed);

    asteroid->objectType = ASTEROID;
    asteroid->request = UPDATE;
    asteroid->updatePosition = true;
    asteroid->draw = true;
    asteroid->isRotatableByGame = true;
    asteroid->objPtr = objPtr;
    asteroid->collider = InitCollider(asteroid->objPtr->shape.sizeMult, *Bounce);
}

void CreateProjectile(ObjectTracker *tracker, ObjectWrap *parent) {

    ObjectWrap *projectile = malloc(sizeof(ObjectWrap));
    projectile[0] = InitWrap();
    if (AddWrapToList(tracker, projectile)) {
        errno = 0;
        free(projectile); // TODO: Make a propper logger and call it
                          // when something goes wrong
        return;
    }

    ObjectStruct *objPtr = malloc(sizeof(ObjectStruct));
    objPtr[0] = InitObject(
        InitShape(PROJECTILE_SHAPE_POINTS,
                  sizeof(PROJECTILE_SHAPE_POINTS) / sizeof(Vector2),
                  PROJECTILE_SIZE),
        (Vector2){ parent->objPtr->position.x +
                       (parent->objPtr->shape.points[0].x * 2.0f),
                   parent->objPtr->position.y +
                       (parent->objPtr->shape.points[0].y * 2.0f) },
        (Vector2){ parent->objPtr->shape.points[0].x * PROJECTILE_SPEED +
                       parent->objPtr->speed.x,
                   parent->objPtr->shape.points[0].y * PROJECTILE_SPEED +
                       parent->objPtr->speed.y },
        5);

    projectile[0] = InitWrap();

    projectile->objectType = PROJECTILE;
    projectile->request = UPDATE;
    projectile->updatePosition = true;
    projectile->draw = true;
    projectile->isRotatableByGame = true;
    projectile->objPtr = objPtr;
    projectile->collider = InitCollider(projectile->objPtr->shape.sizeMult, *GetShot);

}

void DeleteObjWrap(ObjectWrap *wrap) {
    DeleteObjectStruct(wrap->objPtr);
    free(wrap->collider.collidedList);
    free(wrap);
}

void DeleteTrackedObject(ObjectTracker *tracker, unsigned long index) {
    DeleteObjWrap(tracker->objList[index]);
    tracker->objList[index] = 0;
}

void DeleteTracker(ObjectTracker *tracker) {
    for ( unsigned long i = 0; i < tracker->objListLen; i++ ) {
        if ( tracker->objList[i] ) DeleteTrackedObject(tracker, i);
    }
    free(tracker->objList);
    free(tracker);
}
