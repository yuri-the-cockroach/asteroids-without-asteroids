// system includes
#include <stdlib.h>
#include <errno.h>

// local includes
#include "objecthandler.h"
#include "asteroid.h"
#include "autils.h"
#include "collision.h"
#include "logger.h"
#include "objectlogic.h"
#include "structs.h"
#ifdef BENCHMARKING
    #include "benchmarking.h"
#endif // BENCHMARKING

// Returns a list of objects that need to be drawn
void RunActionList(ObjectTracker *tracker) {

#ifdef BENCHMARKING
    long benchmarkStart = 0;
    long sortCalledAt = 0;
    BENCH_COLLIDER_TIME = 0;
    LOG(BENCH, "%s", "<- Starting action list ->");

    BenchStart(&benchmarkStart);
    BenchStart(&sortCalledAt);
#endif // BENCHMARKING

    SortListByX(tracker);

#ifdef BENCHMARKING
    BenchEnd(&sortCalledAt, "Sorter");
#endif

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
                continue;
        }
    }

    #ifdef BENCHMARKING
        if (BENCHRUNNING) {
            LOG(BENCH, "Collider took %ldus for %d objects", BENCH_COLLIDER_TIME, tracker->objListLen);
            BENCH_COLLIDER_TIME = 0;
        }

        if (BENCHRUNNING) LOG(BENCH, "<- Action list finished in %ldus ->", GetTimeMicS() - benchmarkStart);
    #endif

    CleanupMemory(tracker);
}

void UpdateObj(ObjectTracker *tracker, unsigned long index) {

    ObjectWrap *wrap = tracker->objList[index];
    long start = 0;

    #ifdef BENCHMARKING
        if ( BENCHRUNNING ) start = GetTimeMicS();
        if ( wrap->collider.isCollidable)
            FastFindCollisions(tracker, index);
        if ( BENCHRUNNING ) BENCH_COLLIDER_TIME += (GetTimeMicS() - start);
    #endif

    if (wrap->isRotatableByGame) {
        RotateObject(wrap, (wrap->objPtr->rotateSpeed));
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
        0,
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
        0,
        {
        false,
        { 0, 0, 0, 0 },
        0,
        NULL
        },
        0
    };
}

int AddWrapToList(ObjectTracker *tracker, ObjectWrap *wrap) {

    if (!wrap) {
        errno = EFAULT;
        LOG(ERROR, "Got null pointer instead of wrap. errno: %d", tracker->objListLen, errno);
        return errno;
    }

    if (wrap->objectType == ASTEROID && tracker->objListLen >= SOFT_MAX_ASTEROIDS) {
        errno = ECHRNG;
        LOG(ERROR, "Too many asteroids tracker->objListLen is at %ld. errno: %d", tracker->objListLen, errno);
        return errno;
    }

    if (tracker->objListLen >= MAX_OBJECT_COUNT) {
        errno = ECHRNG;
        LOG(ERROR, "Too many objects tracker->objListLen is at %ld. errno: %d", tracker->objListLen, errno);
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

    player->request = CREATE;
    player->isRotatableByGame = false;
    player->updatePosition = true;
    player->draw = true;
    player->objPtr = objPtr;

    player->collider = InitCollider(player->objPtr->shape.sizeMult, *PlayerCollision);
    player->collider.mass = 1;
    player->livesLeft = 2;

    // Camera stuff
    tracker->playerCamera.target = (Vector2){ tracker->playerPtr->objPtr->position.x,
                               tracker->playerPtr->objPtr->position.y };
    tracker->playerCamera.offset =
        (Vector2){ (float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f };

    tracker->playerCamera.rotation = 0.0f;
    tracker->playerCamera.zoom = 1.0f;


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
        parent->objPtr->shape.points[0].x * 2.0f ,     // I don't know why this works, but other approach doesn't
        parent->objPtr->position.y +                   // and at this point I don't care.
        parent->objPtr->shape.points[0].y * 2.0f  },   // Just don't change it, keep it the way it is
        (Vector2){ parent->objPtr->shape.points[0].x * PROJECTILE_SPEED +
                       parent->objPtr->speed.x,
                   parent->objPtr->shape.points[0].y * PROJECTILE_SPEED +
                       parent->objPtr->speed.y },
        5);

    projectile[0] = InitWrap();

    projectile->objectType = PROJECTILE;
    projectile->request = CREATE;
    projectile->updatePosition = true;
    projectile->draw = true;
    projectile->isRotatableByGame = true;
    projectile->objPtr = objPtr;
    projectile->collider = InitCollider(projectile->objPtr->shape.sizeMult, *GetShot);

}

void DeleteObjWrap(ObjectWrap *wrap) {
    DeleteObjectStruct(wrap->objPtr);
    free((void *)wrap);
}

void DeleteTrackedObject(ObjectTracker *tracker, unsigned long index) {
    DeleteObjWrap((void *)tracker->objList[index]);
    tracker->objList[index] = 0;
}

void DeleteTracker(ObjectTracker *tracker) {
    if ( !tracker ) return;
    CleanupMemory(tracker);
    for ( unsigned long i = 0; i < tracker->objListLen; i++ ) {
        if ( tracker->objList[i] ) tracker->objList[i]->request = DELETE;
    }
    RunActionList(tracker);
    free((void *)tracker->objList);
    free((void *)tracker);
}
