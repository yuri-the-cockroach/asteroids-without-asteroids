// system includes
#include <errno.h>
#include <stdlib.h>

// local includes
#include "asteroid.h"
#include "autils.h"
#include "collision.h"
#include "logger.h"
#include "objecthandler.h"
#include "objectlogic.h"
#include "render.h"
#include "structs.h"
#include "vector-math.h"

// Returns a list of objects that need to be drawn
void RunActionList(objTracker *tracker) {

    SortListByX(tracker);

    // Run through all the tracked objects
    for (unsigned long i = 0; i < tracker->objListLen; i++) {

        // Check if current object is NULLed (it really shouldn't be)
        if (tracker->objList[i] == 0) continue;
        // If it is somehow NULL, then cleanup:
        // Check if there's any elements ahead at all. If not just decrement
        // the length of the list.

        objWrap *current = tracker->objList[i];
        switch (current->request) {

        case CREATE:
            current->request = UPDATE;
            break;

        case DELETE: // Delete element from the list
            DeleteTrackedObject(tracker, i);
            break;

        case SEPARATE:
            Separate(tracker, tracker->objList[i]);
            i--;
            break;

        case UPDATE: // Call the updater function of the element
        {
            UpdateObj(tracker, i);
            break;
        }

        case IGNORE:
            break;
        }
    }

    CleanupMemory(tracker);
}

}

objTracker *InitTracker(void) {
    objTracker *tracker = calloc(1, sizeof(objTracker));
    tracker[0] =
        (objTracker){ NULL,
                      { .zoom = 1 },
                      (objWrap **)calloc(MAX_OBJECT_COUNT, sizeof(objWrap *)),
                      0,
                      0 };
    return tracker;
}

objWrap InitWrap(void) {
    return (objWrap){
        NOTYPE,
        IGNORE,
        false,
        false,
        false,
        0,
        { false, { 0, 0, 0, 0 }, 0, NULL },
        0
    };
}

int AddWrapToList(objTracker *tracker, objWrap *wrap) {

    if (!wrap) {
        errno = EFAULT;
        LOG(ERROR,
            "Got null pointer instead of wrap. errno: %d",
            tracker->objListLen,
            errno);
        return errno;
    }

    if (wrap->objectType == ASTEROID &&
        tracker->objListLen >= SOFT_MAX_ASTEROIDS) {
        errno = ECHRNG;
        LOG(ERROR,
            "Too many asteroids tracker->objListLen is at %ld. errno: %d",
            tracker->objListLen,
            errno);
        return errno;
    }

    if (tracker->objListLen >= MAX_OBJECT_COUNT) {
        errno = ECHRNG;
        LOG(ERROR,
            "Too many objects tracker->objListLen is at %ld. errno: %d",
            tracker->objListLen,
            errno);
        return errno;
    }

    if (wrap->objectType == PLAYER) {
        if (tracker->playerPtr) {
            errno = EADDRINUSE;
            LOG(ERROR,
                "Player is already present in the list. errno: %d",
                errno);
            return errno;
        }
        tracker->playerPtr = wrap;
    }

    tracker->objList[tracker->objListLen] = wrap;
    tracker->objListLen++;
    return 0;
}

// Pass the default speed, default rotation speed, default position
void CreatePlayer(objTracker *tracker, Vector2 initPosition, float size) {

    objWrap *player    = malloc(sizeof(objWrap));
    player[0]          = InitWrap();
    player->objectType = PLAYER;

    if (AddWrapToList(tracker, player)) {
        free(player);
        return;
    }

    object *objPtr = malloc(sizeof(object));
    objPtr[0] =
        InitObject(InitShape(PLAYER_SHAPE_POINTS,
                             sizeof(PLAYER_SHAPE_POINTS) / sizeof(Vector2),
                             size),
                   initPosition,
                   (Vector2){ 0, 0 },
                   0);

    player->request           = CREATE;
    player->isRotatableByGame = false;
    player->updatePosition    = true;
    player->draw              = true;
    player->objPtr            = objPtr;

    player->collider =
        InitCollider(player->objPtr->shape.sizeMult, *PlayerCollision);
    player->collider.mass = 1;
    player->livesLeft     = 2;

    // Camera stuff
    tracker->playerCamera.target =
        (Vector2){ tracker->playerPtr->objPtr->position.x,
                   tracker->playerPtr->objPtr->position.y };
    tracker->playerCamera.offset =
        (Vector2){ (float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f };

    tracker->playerCamera.rotation = 0.0f;
    tracker->playerCamera.zoom     = 1.0f;
}

void CreateProjectile(objTracker *tracker, objWrap *parent) {

    objWrap *projectile = malloc(sizeof(objWrap));
    projectile[0]       = InitWrap();
    if (AddWrapToList(tracker, projectile)) {
        errno = 0;
        free(projectile);
        return;
    }

    object *objPtr = malloc(sizeof(object));
    objPtr[0]      = InitObject(
        InitShape(PROJECTILE_SHAPE_POINTS,
                  sizeof(PROJECTILE_SHAPE_POINTS) / sizeof(Vector2),
                  PROJECTILE_SIZE),

        // I don't know why this works, but other approach
        // doesn't and at this point I don't care. Just don't
        // change it, keep it the way it is
        VecAddVec(VecMulFloat(parent->objPtr->shape.points[0], 3.0f),
                  parent->objPtr->position),
        VecAddVec(
            VecMulFloat(parent->objPtr->shape.points[0], PROJECTILE_SPEED),
            parent->objPtr->speed),
        5);

    projectile[0] = InitWrap();

    projectile->objectType        = PROJECTILE;
    projectile->request           = CREATE;
    projectile->updatePosition    = true;
    projectile->draw              = true;
    projectile->isRotatableByGame = true;
    projectile->objPtr            = objPtr;
    projectile->collider =
        InitCollider(projectile->objPtr->shape.sizeMult, *GetShot);
}

void DeleteObjWrap(objWrap *wrap) {
    DeleteObjectStruct(wrap->objPtr);
    free((void *)wrap);
}

void DeleteTrackedObject(objTracker *tracker, unsigned long index) {
    DeleteObjWrap((void *)tracker->objList[index]);
    tracker->objList[index] = 0;
}

void DeleteTracker(objTracker *tracker) {
    if (!tracker) return;
    CleanupMemory(tracker);
    for (unsigned long i = 0; i < tracker->objListLen; i++) {
        if (tracker->objList[i]) tracker->objList[i]->request = DELETE;
    }
    RunActionList(tracker);
    free((void *)tracker->objList);
    free((void *)tracker);
}
