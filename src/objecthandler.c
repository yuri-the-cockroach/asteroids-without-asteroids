#include "objecthandler.h"

#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "gamelogic.h"
#include "objectlogic.h"
#include "render.h"
#include "utils.h"

// Returns a list of objects that need to be drawn
void RunActionList(ObjectTracker *self) {

    // Run through all the tracked objects
    for (unsigned long i = 0; i < self->objListLen; i++) {
        ObjectWrap *current = self->objList[i];

        // Check if current object is NULLed (it really shouldn't be)
        if (current == NULL) {
            // If it is somehow NULL, then cleanup:
            // Check if there's any elements ahead at all. If not just decrement the length of the list.
            if (i > self->objListLen - 2) {
                self->objListLen--;
                break;
            }

            self->objList[i] = self->objList[self->objListLen - 1]; // Set current to the last known element of the list
            self->objList[self->objListLen] = 0; // NULL the last one
            self->objListLen--; // Decrement the length
            current = self->objList[i]; // Set current to the newly assinged element
        }

        switch (current->request) {
        case IGNORE:
            continue; // next for cycle

        case DELETE: // Delete element from the list
            DeleteTrackedObject(self, current);
            continue;

        case UPDATE: // Call the updater function of the element
            UpdateObj(self, current);
            break;
        }
    }
}

void UpdateObj(ObjectTracker *tracker, ObjectWrap *wrap) {
    if (wrap->player)
        KeyboardHandler(wrap->objPtr);

    if (wrap->collider)
        CheckCollisions(tracker, wrap);

    if (wrap->updatePosition) {
        if (!wrap->ignoreNext) {
            UpdateObjectPos(wrap->objPtr);
        } else
            wrap->ignoreNext = false;
    }

    if (wrap->isRotatableByGame) {
        RotateObject(wrap->objPtr, (wrap->objPtr->rotateSpeed));
    }

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
        ObjectWrap){ 0, IGNORE, false, false, false, false, 0, NULL, false, NULL};
}

void AddWrapToList(ObjectTracker *self, ObjectWrap *wrap) {
    if (self->objListLen >= MAX_OBJECT_COUNT) {
        fprintf(stderr,
                "Error in AddWrapToList, too many objects in "
                "self->objList\nself->objListLen is at %ld\n",
                self->objListLen);
        /* exit(ECHRNG); */
        return;
    }
    wrap->index = self->objListLen;
    self->objList[self->objListLen] = wrap;
    self->objListLen++;
}

// Pass the default speed, default rotation speed, default position
void CreatePlayer(ObjectTracker *self, Vector2 initPosition,
                  unsigned int accelSpeed, unsigned int rotSpeed) {
    ObjectWrap *player = malloc(sizeof(ObjectWrap));
    player[0] = InitWrap();

    player->request = UPDATE;
    player->player = true;
    player->updatePosition = true;
    player->draw = true;
    player->collider = true;

    player->objPtr = malloc(sizeof(ObjectStruct));
    player->objPtr[0] =
        InitObject(InitShape(PLAYER_SHAPE_POINTS,
                             sizeof(PLAYER_SHAPE_POINTS) / sizeof(Vector2),
                             0.5),
                   initPosition,
                   (Vector2){ 0, 0 },
                   accelSpeed,
                   rotSpeed);
    AddWrapToList(self, player);
}

void CreateAsteroid(ObjectTracker *self, Vector2 initPosition,
                    Vector2 initSpeed, int constRotationSpeed) {
    ObjectWrap *asteroid = malloc(sizeof(ObjectWrap));
    asteroid[0] = InitWrap();
    asteroid->request = UPDATE;
    asteroid->updatePosition = true;
    asteroid->draw = true;
    asteroid->collider = true;
    asteroid->cRotSpeed = constRotationSpeed;
    asteroid->objPtr = malloc(sizeof(ObjectStruct));
    asteroid->objPtr[0] =
        InitObject(InitShape(ASTEROID_SHAPE_POINTS,
                             sizeof(ASTEROID_SHAPE_POINTS) / sizeof(Vector2),
                             0.5),
                   initPosition,
                   initSpeed,
                   0,
                   0);
    AddWrapToList(self, asteroid);
}

void DeleteObjWrap(ObjectWrap *self) {
    DeleteObjectStruct(self->objPtr);
    free(self);
}

void DeleteTrackedObject(ObjectTracker *self, ObjectWrap *wrap) {
    unsigned long index = wrap->index;
    wrap->WrapDestructor(wrap);

    self->objListLen--;
    if (self->objListLen == index) {
        self->objList[index] = 0;
        return;
    }

    self->objList[index] = self->objList[self->objListLen];
    self->objList[self->objListLen] = 0;
}

void CheckCollisions(ObjectTracker *tracker, ObjectWrap *first) {
    if (tracker->objListLen < 2)
        return;

    for (unsigned int j = 0; j < tracker->objListLen; j++) {

        ObjectWrap *second = tracker->objList[j];

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
            Bounce(first, second);
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
            clamp(second->objPtr->mass / first->objPtr->mass, 0, 1);
        second->objPtr->speed.x +=
            (relativeSpeed.x *
             clamp(first->objPtr->mass / second->objPtr->mass, 0, 1));

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
        if (left->isRotatableByGame){
            ApplyMassBasedRandRotation(left);
        }
        if (right->isRotatableByGame){
            ApplyMassBasedRandRotation(right);
        }
    }

    if (first->objPtr->position.x - second->objPtr->position.x <=
        first->objPtr->position.y - second->objPtr->position.y)
    {
        first->objPtr->speed.y -=
            (relativeSpeed.y *
             clamp(second->objPtr->mass / first->objPtr->mass, 0, 1));
        second->objPtr->speed.y +=
            (relativeSpeed.y *
             clamp(first->objPtr->mass / second->objPtr->mass, 0, 1));

        ObjectWrap *above, *below;

        if (first->objPtr->position.y < second->objPtr->position.y) {
            above = first;
            below = second;
        } else {
            above = second;
            below = first;
        }

        if (first->objPtr->position.y != second->objPtr->position.y) {
            below->objPtr->position.y += 1;
            above->objPtr->position.y -= 1;
        }
    }
}
