#include "objecthandler.h"
#include "asteroid.h"
#include "asteroidsutils.h"
#include "collision.h"
#include "logger.h"
#include "objectlogic.h"
#include "structs.h"
#include "benchmarking.h"

// Returns a list of objects that need to be drawn
void RunActionList(ObjectTracker *tracker) {
    long benchmarkStart = 0;
    long sortCalledAt = 0;
    BENCH_COLLIDER_TIME = 0;
        LOG(BENCH, "%s", "<- Starting action list ->");

    BenchStart(&benchmarkStart);

    BenchStart(&sortCalledAt);
    SortListByX(tracker);
    BenchEnd(&sortCalledAt, "Sorter");

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
    if (BENCHMARKING) {
        LOG(BENCH, "Collider took %ldus for %d objects", BENCH_COLLIDER_TIME, tracker->objListLen);
        BENCH_COLLIDER_TIME = 0;
    }

    if (BENCHMARKING) LOG(BENCH, "<- Action list finished in %ldus ->", GetTimeMicS() - benchmarkStart);

    CleanupMemory(tracker);
    CleanupLists(tracker);
}

void UpdateObj(ObjectTracker *tracker, unsigned long index) {

    ObjectWrap *wrap = tracker->objList[index];
    long start = 0;

    if ( BENCHMARKING ) start = GetTimeMicS();
    if ( wrap->collider.isCollidable)
        FastFindCollisions(tracker, i);
    if ( BENCHMARKING ) BENCH_COLLIDER_TIME += (GetTimeMicS() - start);

    /* long start = 0; */

    /* if ( BENCHMARKING ) start = GetTimeMicS(); */
    /* if ( wrap->collider.isCollidable) */
    /*     FindCollisions(tracker, wrap); */
    /* if ( BENCHMARKING ) BENCH_COLLIDER_TIME += (GetTimeMicS() - start); */

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
        false,
        {
        false,
        { 0, 0, 0, 0 },
        0,
        NULL,
        0,
        NULL},
        0
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

    player->collider = InitCollider(player->objPtr->shape.sizeMult, *PlayerCollision);
    player->collider.mass = 1;
    player->livesLeft = 2;

    // Camera stuff
    tracker->playerCamera.target = (Vector2){ tracker->objList[0]->objPtr->position.x + 20.0f,
                               tracker->objList[0]->objPtr->position.y + 20.0f };
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
    CleanupMemory(tracker);
    for ( unsigned long i = 0; i < tracker->objListLen; i++ ) {
        if ( tracker->objList[i] ) tracker->objList[i]->request = DELETE;
    }
    RunActionList(tracker);
    free(tracker->objList);
    free(tracker);
}
