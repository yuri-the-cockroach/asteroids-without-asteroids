// system includes
#include <stdlib.h>

// local includes
#include "collision.h"
#include "autils.h"
#include "structs.h"

bool FindAnyCollision(objTracker *tracker, objWrap *first) {
    if (tracker->objListLen < 2)
        return false;

    if (!first)
        return false;

    for (unsigned int j = 0; j < tracker->objListLen; j++) {
        objWrap *second = tracker->objList[j];
        if (tracker->objList[j] == NULL)
            continue;

        if (first == second)
            continue;

        if (!second->collider.isCollidable)
            continue;

        if (CheckIfCollide(first, second))
            return true;
    }
    return false;
}

objWrap *FindCollisionPos(objTracker *tracker, Vector2 pos) {
    if (tracker->objListLen < 1)
        return NULL;

    for (unsigned int j = 0; j < tracker->objListLen; j++) {
        objWrap *second = tracker->objList[j];
        if (tracker->objList[j] == NULL)
            continue;

        if (!second->collider.isCollidable)
            continue;


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

        if ((pos.x < secondEnd.x && secondStart.x < pos.x) &&
            (pos.y < secondEnd.y && secondStart.y < pos.y))
            return second;
    }
    return NULL;
}


void SortListByX(objTracker *tracker) {
    unsigned long i = 1;
    unsigned long gotToPos = 0;
    while (i < tracker->objListLen) {

        objWrap *prev = tracker->objList[i - 1];
        objWrap *current = tracker->objList[i];
        if (current->objPtr->position.x >= prev->objPtr->position.x) {
            if (i > gotToPos) {
                gotToPos = i;
            } else
                i = gotToPos;
            i++;
            continue;
        }
        tracker->objList[i] = prev;
        tracker->objList[i - 1] = current;

        if (i > 1)
            i--;
    }
}

void FastFindCollisions(objTracker *tracker, unsigned long index) {
    objWrap *current = tracker->objList[index];
    if (tracker->objListLen < 2)
        return;

    if (!current)
        return;

    if (current->request == DELETE)
        return;

    objWrap *next;

    for (unsigned long j = index + 1; j < tracker->objListLen; j++) {
        next = tracker->objList[j];

        if (tracker->objList[j] == NULL || current == next ||
            next->request != UPDATE || !next->collider.isCollidable)
            continue;

        if (current->collider.collider.x + current->collider.collider.width <=
            next->collider.collider.x)
            break;
        if (!CheckIfCollide(current, next))
            continue;

        if ( current->objectType == next->objectType && current->objectType == PROJECTILE ) continue;

        if (current->objectType > next->objectType) {
            current->collider.ActionOnCollision(tracker, current, next);
            continue;
        }
        next->collider.ActionOnCollision(tracker, next, current);
    }
    return;
}

void ApplyMassBasedRandRotation(objWrap *wrap) {

    float massMod = 1 / wrap->collider.mass;
    float randomMod = (float)(GetRandomValue(-1000, 1000)) / 1000;

    wrap->objPtr->rotateSpeed = 2 * randomMod * massMod;
}

void Bounce(objTracker *tracker, objWrap *first, objWrap *second) {

    UNUSED(tracker);

    objWrap *left, *right;
    objWrap *above, *below;

    float frameTime = GetFrameTime();

    // If A.x < B.x then A is left of B
    left = first->objPtr->position.x < second->objPtr->position.x
               ? first
               : second;                    // Find the left one
    right = left == first ? second : first; // Set right to the other one

    // If A.y < B.y then A is above of B
    above = first->objPtr->position.y < second->objPtr->position.y
                ? first
                : second;                    // Find the above one
    below = above == first ? second : first; // Set below to the other one
    {

        // I wasted 24 fucking hours trying to make this work
        float Sa = first->objPtr->speed.x;
        float Sb = second->objPtr->speed.x;

        float ma = ClampFloat(first->collider.mass, 1, 1024);
        float mb = ClampFloat(second->collider.mass, 1, 1024);

        first->objPtr->speed.x = Sa + (Sb - Sa) / ma;
        second->objPtr->speed.x = Sb + (Sa - Sb) / mb;

        LOG(TRACE, "MASSES:\n A == %f\n B == %f", ma, mb);
        LOG(TRACE, "SPEEDS BEFORE:\n First == %f\n Second == %f", Sa, Sb);
        LOG(TRACE,
            "SPEEDS AFTER:\n First == %f\n Second == %f",
            first->objPtr->speed.x,
            second->objPtr->speed.x);

        left->objPtr->position.x -= BOUCE_CONSTANT * frameTime;
        right->objPtr->position.x += BOUCE_CONSTANT * frameTime;
    }

    {
        float Sa = first->objPtr->speed.y;
        float Sb = second->objPtr->speed.y;

        float ma = ClampFloat(first->collider.mass, 1, 1024);
        float mb = ClampFloat(second->collider.mass, 1, 1024);

        first->objPtr->speed.y = Sa + (Sb - Sa) / ma;
        second->objPtr->speed.y = Sb + (Sa - Sb) / mb;

        LOG(TRACE, "MASSES:\n A == %f\n B == %f", ma, mb);
        LOG(TRACE, "SPEEDS BEFORE:\n First == %f\n Second == %f", Sa, Sb);
        LOG(TRACE,
            "SPEEDS AFTER:\n First == %f\n Second == %f",
            first->objPtr->speed.y,
            second->objPtr->speed.y);

        above->objPtr->position.y -= BOUCE_CONSTANT * frameTime;
        below->objPtr->position.y += BOUCE_CONSTANT * frameTime;
    }
    return;
}

bool CheckIfCollide(objWrap *first, objWrap *second) {
    // This abomination will make the logic actually readable
    Vector2 firstStart = {
        first->objPtr->position.x +     // leftmost point of the collider
            first->collider.collider.x, // in the abs coord system

        first->objPtr->position.y +    // topmost point of the collider
            first->collider.collider.y // in the abs coord system
    };
    Vector2 firstEnd = { firstStart.x + // The rightmost point
                             first->collider.collider.width,
                         first->objPtr->position.y +
                             first->collider.collider.y +
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

void GetShot(objTracker *tracker, objWrap *projectile,
             objWrap *victim) {

    projectile->request = DELETE;
    if (victim->objectType == PROJECTILE)
        victim->request = DELETE;

    if (victim->objectType == ASTEROID) {
        victim->livesLeft--;
        victim->request = SEPARATE;
        tracker->playerScore += 1 * CUR_DIFFICULTY;
    }
}

collider InitCollider(float sizeMult,
                      void (*ActionOnCollision)(objTracker *tracker,
                                                objWrap *first,
                                                objWrap *second)) {
    return (collider){
        true,
        (Rectangle){
                    -50 * sizeMult,
                    -50 * sizeMult,
                    100 * sizeMult,
                    100 * sizeMult,
                    },
        sizeMult * sizeMult,
        ActionOnCollision
    };
}

int UpdateCollider(objWrap *wrap) {

    float leftMostPoint = 10000;
    float rightMostPoint = -10000;
    float topMostPoint = 10000;
    float bottomMostPoint = -10000;

    for (unsigned int i = 0; i < wrap->objPtr->shape.arrayLength; i++) {
        leftMostPoint = wrap->objPtr->shape.points[i].x < leftMostPoint
                            ? wrap->objPtr->shape.points[i].x
                            : leftMostPoint;
        rightMostPoint = wrap->objPtr->shape.points[i].x > rightMostPoint
                             ? wrap->objPtr->shape.points[i].x
                             : rightMostPoint;
        topMostPoint = wrap->objPtr->shape.points[i].y < topMostPoint
                           ? wrap->objPtr->shape.points[i].y
                           : topMostPoint;
        bottomMostPoint = wrap->objPtr->shape.points[i].y > bottomMostPoint
                              ? wrap->objPtr->shape.points[i].y
                              : bottomMostPoint;
    }

    wrap->collider.collider.x = leftMostPoint * 0.9f;
    wrap->collider.collider.y = topMostPoint * 0.9f;
    wrap->collider.collider.width =
        fabsf(leftMostPoint - rightMostPoint) * 0.9f;
    wrap->collider.collider.height =
        fabsf(topMostPoint - bottomMostPoint) * 0.9f;
    return 0;
}

void PlayerCollision(objTracker *tracker, objWrap *player,
                     objWrap *offender) {
    player->livesLeft--;

    if (!player->livesLeft) {
        GAME_STATE = GAME_OVER;
        return;
    }

    if (offender->objectType == PROJECTILE)
        offender->request = DELETE;
    if (offender->objectType == ASTEROID)
        Bounce(tracker, player, offender);

    return;
}
