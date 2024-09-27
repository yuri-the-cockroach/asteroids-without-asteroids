#include "collision.h"
#include "asteroidsutils.h"
#pragma GCC diagnostic ignored "-Wdouble-promotion"

bool FindAnyCollision(ObjectTracker *tracker, ObjectWrap *first) {
    if (tracker->objListLen < 2)
        return false;

    if (!first)
        return false;

    for (unsigned int j = 0; j < tracker->objListLen; j++) {
        ObjectWrap *second = tracker->objList[j];
        if (tracker->objList[j] == NULL)
            continue;

        if (first == second)
            continue;

        if (!second->collider.isCollidable)
            continue;

        if (CheckIfCollide(first, second)) return true;
    }
    return false;
}

void FindCollisions(ObjectTracker *tracker, ObjectWrap *first) {
    if (tracker->objListLen < 2)
        return;

    if (!first)
        return;

    for (unsigned int j = 0; j < tracker->objListLen; j++) {
        ObjectWrap *second = tracker->objList[j];
        if (tracker->objList[j] == NULL)
            continue;

        if (first == second)
            continue;
        if (!second->collider.isCollidable)
            continue;

        if (FindInList(&first->collider, second) != -1) continue;

        if (!CheckIfCollide(first, second))
            continue;

        Push(&first->collider, second);
        Push(&second->collider, first);

        if (first->objectType > second->objectType) {
            first->collider.ActionOnCollision(first, second);
            continue;
        }
        second->collider.ActionOnCollision(first, second);


    }
}

void ApplyMassBasedRandRotation(ObjectWrap *wrap) {

    float massMod = 1 / wrap->collider.mass;
    float randomMod = (float)(GetRandomValue(-1000, 1000)) / 1000;

    wrap->objPtr->rotateSpeed = 2 * randomMod * massMod;
}

void Bounce(ObjectWrap *first, ObjectWrap *second) {

    ObjectWrap *left, *right;
    ObjectWrap *above, *below;

    float frameTime = GetFrameTime();

    // If A.x < B.x then A is left of B
    left = first->objPtr->position.x < second->objPtr->position.x ? first : second; // Find the left one
    right = left == first ? second : first; // Set right to the other one

    // If A.y < B.y then A is above of B
    above = first->objPtr->position.y < second->objPtr->position.y ? first : second; // Find the above one
    below = above == first ? second : first; // Set below to the other one
    {

        // I wasted 24 fucking hours trying to make this work
        float Sa = first->objPtr->speed.x;
        float Sb = second->objPtr->speed.x;

        float ma = ClampFloat(first->collider.mass, 1, 1024);
        float mb = ClampFloat(second->collider.mass, 1, 1024);

        first->objPtr->speed.x = Sa + (( Sb - Sa ) / ma) * 0.5f;
        second->objPtr->speed.x = Sb + (( Sa - Sb ) / mb) * 0.5f;

        LOG(TRACE, "MASSES:\n A == %f\n B == %f", ma, mb);
        LOG(TRACE, "SPEEDS BEFORE:\n First == %f\n Second == %f", Sa, Sb);
        LOG(TRACE, "SPEEDS AFTER:\n First == %f\n Second == %f", first->objPtr->speed.x, second->objPtr->speed.x);

        left->objPtr->position.x -= BOUCE_CONSTANT * frameTime;
        right->objPtr->position.x += BOUCE_CONSTANT * frameTime;
    }

    {
        float Sa = first->objPtr->speed.y;
        float Sb = second->objPtr->speed.y;

        float ma = ClampFloat(first->collider.mass, 1, 1024);
        float mb = ClampFloat(second->collider.mass, 1, 1024);

        first->objPtr->speed.y = Sa + ( Sb - Sa ) / ma;
        second->objPtr->speed.y = Sb + ( Sa - Sb ) / mb;

        first->objPtr->speed.y *= 0.9f;
        second->objPtr->speed.y *= 0.9f;

        LOG(TRACE, "MASSES:\n A == %f\n B == %f", ma, mb);
        LOG(TRACE, "SPEEDS BEFORE:\n First == %f\n Second == %f", Sa, Sb);
        LOG(TRACE, "SPEEDS AFTER:\n First == %f\n Second == %f", first->objPtr->speed.y, second->objPtr->speed.y);

        above->objPtr->position.y -= BOUCE_CONSTANT * frameTime;
        below->objPtr->position.y += BOUCE_CONSTANT * frameTime;
    }
    return;
}

bool CheckIfCollide(ObjectWrap *first, ObjectWrap *second) {

    // This abomination will make the logic actually readable
    Vector2 firstStart = {
        first->objPtr->position.x +    // leftmost point of the collider
            first->collider.collider.x, // in the abs coord system

        first->objPtr->position.y +   // topmost point of the collider
            first->collider.collider.y // in the abs coord system
    };
    Vector2 firstEnd = { firstStart.x + // The rightmost point
                             first->collider.collider.width,
                         first->objPtr->position.y + first->collider.collider.y +
                             first->collider.collider.height };
    Vector2 secondStart = {
        second->objPtr->position.x + second->collider.collider.x,
        second->objPtr->position.y + second->collider.collider.y
    };
    Vector2 secondEnd = { second->objPtr->position.x +
                              second->collider.collider.x +
                              second->collider.collider.width,
                          second->objPtr->position.y +
                              second->collider.collider.y +
                              second->collider.collider.height };

    // Explanation:
    // Assume x == 0 is the leftmost point of the system
    // And y == 0 is the topmost point of the system
    //
    // firstStart.x -- Leftmost point of the first object
    // firstEnd.x -- Rightmost point of the first object
    // secondStart.x -- Leftmost point of the second object
    // secondEnd.x -- Rightmost point of the second object
    //
    // |--------| <- first object
    //      |--------| <- second object
    //
    // firstStart.y -- Topmost point of the first object
    // firstEnd.y -- Bottommost point of the first object
    // secondStart.y -- Topmost point of the second object
    // secondEnd.y -- Bottommost point of the second object
    //
    // ___
    //  | <- first object
    //  | ___
    //  |  | <- second object
    //  |  |
    // --- |
    //     |
    //    ---
    //
    // If they overlap in both axis, then they collide
    // Actuall logic
    if ((firstStart.x < secondEnd.x && secondStart.x < firstEnd.x) &&
        (firstStart.y < secondEnd.y && secondStart.y < firstEnd.y))
        return true;

    return false;
}

void GetShot(ObjectWrap *projectile, ObjectWrap *victim) {
    projectile->request = DELETE;
    victim->request = DELETE;
    if ( victim->objectType == PLAYER ) LOG(WARNING, "%s", "Removing player object");
}

Collider InitCollider(float sizeMult, void (*ActionOnCollision)(ObjectWrap *first, ObjectWrap *second)) {
    return (Collider){
        true,
        (Rectangle){ -50 * sizeMult,
                    -50 * sizeMult,
                    100 * sizeMult,
                    100 * sizeMult,},
            sizeMult * sizeMult,
            ActionOnCollision,
            0,
            calloc(MAX_OBJECT_COUNT, sizeof(Collider*))
    };
}

// Push to the end of list
int Push(Collider *parent, ObjectWrap *wrap) {
    if ( parent->collidedListLen > MAX_OBJECT_COUNT - 1 ) {
        LOG(ERROR, "%s", "Cannot push the object to the list, list is already full");
        return -1; // List too long
    }
    parent->collidedList[parent->collidedListLen] = wrap;
    parent->collidedListLen++;
    return parent->collidedListLen - 1;
}

// Find the entry in the list
int FindInList(Collider *parent, ObjectWrap *wrap) {
    for ( int i = 0; i < parent->collidedListLen; i++) {
        if ( parent->collidedList[i] == wrap) return i;
    }
    LOG(ALL, "%s", "Wrap not found in list");
    return -1;
}

// Pop entry from the list
ObjectWrap* Pop(Collider *parent, int index) {
    if ( index > parent->collidedListLen ) return NULL;
    if ( parent->collidedList[index] == NULL) LOG(WARNING, "%s", "Entry is in the list, but is NULL");
    return parent->collidedList[index];
}

// Clear the list
int ClearList(Collider *parent) {
    if ( parent->collidedListLen > MAX_OBJECT_COUNT ) {
        LOG(FATAL, "%s", "List length is more than allowd, cannot proceed");
        err(ERANGE, "List length out of range");
    }

    for ( int i = 0; i < MAX_OBJECT_COUNT; i++) {
       parent->collidedList[i] = NULL;
    }

    parent->collidedListLen = 0;
    return 0;
}

int CleanupLists(ObjectTracker *tracker) {
    for ( unsigned long i = 0; i < tracker->objListLen; i++ ) {
        ClearList(&tracker->objList[i]->collider);
    }
    return 0;
}
