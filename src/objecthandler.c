#include "objecthandler.h"

#include <err.h>
#include <errno.h>
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "gamelogic.h"
#include "objectlogic.h"
#include "render.h"
#include "structs.h"
#include "utils.h"

// Returns a list of objects that need to be drawn
void RunActionList(ObjectTracker *tracker) {

    // Run through all the tracked objects
    for (unsigned long i = 0; i < tracker->objListLen; i++) {

        // Check if current object is NULLed (it really shouldn't be)
        if (tracker->objList[i] == 0) continue;
            // If it is somehow NULL, then cleanup:
            // Check if there's any elements ahead at all. If not just decrement
            // the length of the list.

        ObjectWrap *current = tracker->objList[i];
        switch (current->request) {
        case IGNORE:
            continue; // next for cycle

        case DELETE: // Delete element from the list
            DeleteTrackedObject(tracker, i);
            continue;

        case UPDATE: // Call the updater function of the element
            UpdateObj(tracker, current);
            break;
        }
    }

    CleanupMemory(tracker);
}

void UpdateObj(ObjectTracker *tracker, ObjectWrap *wrap) {
    if (wrap->objectType == PLAYER)
        KeyboardHandler(wrap->objPtr);

    if (wrap->collider)
        CheckCollisions(tracker, wrap);

    if (wrap->isRotatableByGame) {
        RotateObject(wrap->objPtr, (wrap->objPtr->rotateSpeed));
    }

    if (wrap->updatePosition)
        UpdateObjectPos(wrap->objPtr);

    if (wrap->draw)
        AddToDrawList(tracker, wrap->objPtr);
}

ObjectTracker InitTracker(void) {
    return (ObjectTracker){
        false,
        (ObjectWrap **)calloc(MAX_OBJECT_COUNT, sizeof(ObjectWrap *)),
        0,
        (ObjectStruct **)calloc(MAX_OBJECT_COUNT, sizeof(ObjectStruct *)),
        0,
    };
}

ObjectWrap InitWrap(void) {
    return (
        ObjectWrap){ NOTYPE, IGNORE, false, false, false, false, NULL, NULL };
}

int AddWrapToList(ObjectTracker *tracker, ObjectWrap *wrap) {
    if (tracker->objListLen >= MAX_OBJECT_COUNT) {
        fprintf(stderr,
                "Error in AddWrapToList, too many objects in "
                "tracker->objList\ntracker->objListLen is at %ld\n",
                tracker->objListLen);
        errno = ECHRNG;
        return -1;
    }
    tracker->objList[tracker->objListLen] = wrap;
    tracker->objListLen++;
    return 0;
}

// Pass the default speed, default rotation speed, default position
void CreatePlayer(ObjectTracker *tracker, Vector2 initPosition, float size) {

    ObjectWrap *player = malloc(sizeof(ObjectWrap));
    player[0] = InitWrap();

    if (AddWrapToList(tracker, player)) {
        free(player); // TODO: Make a propper logger and call it when
                      // something goes wrong
        return;
    }

    ObjectStruct *objPtr = malloc(sizeof(ObjectStruct));
    objPtr[0] =
        InitObject(InitShape(PLAYER_SHAPE_POINTS,
                             sizeof(PLAYER_SHAPE_POINTS) / sizeof(Vector2),
                             size),
                   initPosition,
                   (Vector2){ 0, 0 },
                   0,
                   0,
                   .8);

    player[0] = (ObjectWrap){ PLAYER, UPDATE, true,   true,
                              true,   false, objPtr, *Bounce };
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
                   0,
                   constRotationSpeed,
                   1);

    asteroid[0] = (ObjectWrap){ ASTEROID, UPDATE, true,   true,
                                true,     true,   objPtr, *Bounce };
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
                       (parent->objPtr->shape.points[0].x * 1.5),
                   parent->objPtr->position.y +
                       (parent->objPtr->shape.points[0].y * 1.5) },
        (Vector2){ parent->objPtr->shape.points[0].x * PROJECTILE_SPEED +
                       parent->objPtr->speed.x,
                   parent->objPtr->shape.points[0].y * PROJECTILE_SPEED +
                       parent->objPtr->speed.y },
        5,
        (float)0.8);

    projectile[0] = InitWrap();
    projectile[0] = (ObjectWrap){ PROJECTILE, UPDATE, true,     true,
                                  true,       true,   objPtr, *GetShoot };
}

void DeleteObjWrap(ObjectWrap *wrap) {
    DeleteObjectStruct(wrap->objPtr);
    free(wrap);
}

void DeleteTrackedObject(ObjectTracker *tracker, unsigned long index) {
    DeleteObjWrap(tracker->objList[index]);
    tracker->objList[index] = 0;
}

void CheckCollisions(ObjectTracker *tracker, ObjectWrap *first) {
    if (tracker->objListLen < 2)
        return;

    if (!first) return;

    for (unsigned int j = 0; j < tracker->objListLen; j++) {
        ObjectWrap *second = tracker->objList[j];
        if (tracker->objList[j] == NULL)
            continue;

        if (first == second)
            continue;
        if (!second->collider)
            continue;

        // Make it fucking readable
        Vector2 firstStart = {
            first->objPtr->position.x + first->objPtr->collider.x,
            first->objPtr->position.y + first->objPtr->collider.y
        };
        Vector2 firstEnd = { first->objPtr->position.x +
                                 first->objPtr->collider.x +
                                 first->objPtr->collider.width,
                             first->objPtr->position.y +
                                 first->objPtr->collider.y +
                                 first->objPtr->collider.height };
        Vector2 secondStart = {
            second->objPtr->position.x + second->objPtr->collider.x,
            second->objPtr->position.y + second->objPtr->collider.y
        };
        Vector2 secondEnd = { second->objPtr->position.x +
                                  second->objPtr->collider.x +
                                  second->objPtr->collider.width,
                              second->objPtr->position.y +
                                  second->objPtr->collider.y +
                                  second->objPtr->collider.height };

        // Actuall logic
        if ((firstStart.x < secondEnd.x && // Second is RIGHT of First
             firstEnd.x > secondStart.x) &&
            (firstStart.y < secondEnd.y && firstEnd.y > secondStart.y))
        {
            if (first->objectType > second->objectType)
                first->ActionOnCollision(first, second);
            else
                second->ActionOnCollision(first, second);
        }
    }
}

void ApplyMassBasedRandRotation(ObjectWrap *wrap) {

    float massMod = 1 / wrap->objPtr->mass;
    float randomMod = (float)GetRandomValue(-1000, 1000) / 1000;

    wrap->objPtr->rotateSpeed = 2 * randomMod * massMod;
}

void Bounce(ObjectWrap *first, ObjectWrap *second) {

    Vector2 relativeSpeed = { first->objPtr->speed.x - second->objPtr->speed.x,
                              first->objPtr->speed.y -
                                  second->objPtr->speed.y };

    if (first->objPtr->position.x - second->objPtr->position.x >=
        first->objPtr->position.y - second->objPtr->position.y)
    {
        first->objPtr->speed.x -=
            relativeSpeed.x *
            clampFloat(second->objPtr->mass / first->objPtr->mass, 0, 1);
        second->objPtr->speed.x +=
            (relativeSpeed.x *
             clampFloat(first->objPtr->mass / second->objPtr->mass, 0, 1));

        ObjectWrap *left, *right;

        if (first->objPtr->position.x < second->objPtr->position.x) {
            left = first;
            right = second;
        } else {
            left = second;
            right = first;
        }

        if (first->objPtr->position.x != second->objPtr->position.x) {
            left->objPtr->position.x -= 1;
            right->objPtr->position.x += 1;
        }
        if (left->isRotatableByGame) {
            ApplyMassBasedRandRotation(left);
        }
        if (right->isRotatableByGame) {
            ApplyMassBasedRandRotation(right);
        }
    }

    if (first->objPtr->position.x - second->objPtr->position.x <=
        first->objPtr->position.y - second->objPtr->position.y)
    {
        first->objPtr->speed.y -=
            (relativeSpeed.y *
             clampFloat(second->objPtr->mass / first->objPtr->mass, 0, 1));
        second->objPtr->speed.y +=
            (relativeSpeed.y *
             clampFloat(first->objPtr->mass / second->objPtr->mass, 0, 1));

        ObjectWrap *above, *below;

        if (first->objPtr->position.y < second->objPtr->position.y) {
            above = first;
            below = second;
        } else {
            above = second;
            below = first;
        }

        if (roundf(first->objPtr->position.y) != roundf(second->objPtr->position.y)) {
            below->objPtr->position.y += 1;
            above->objPtr->position.y -= 1;
        }
    }
}

void GetShoot(ObjectWrap *projectile, ObjectWrap *victim) {
    projectile->request = DELETE;
    victim->request = DELETE;
}

void CleanupMemory(ObjectTracker *tracker) {
    for (unsigned long i = 0; i < tracker->objListLen; i++) {
        if (tracker->objList[i] != 0)
            continue;

        if (tracker->objList[tracker->objListLen - 1] == 0) {
            tracker->objListLen--;
            i = 0;
            continue;
        }

        tracker->objList[i] = tracker->objList[tracker->objListLen - 1];
        tracker->objList[tracker->objListLen - 1] = 0;
        tracker->objListLen--;
    }
}
