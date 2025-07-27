// system includes
#include "tracingtools.c"
#include <errno.h>
#include <immintrin.h>
#include <pthread.h>
#include <stdlib.h>

// local includes
#include "autils.h"
#include "collision.h"
#include "logger.h"
#include "structs.h"
#include "vector-math.h"

bool FindAnyCollision(objTracker *tracker, objWrap *first) {
    if (tracker->objListLen < 2) return false;

    if (!first) return false;

    for (unsigned int j = 0; j < tracker->objListLen; j++) {
        objWrap *second = tracker->objList[j];
        if (tracker->objList[j] == NULL) continue;

        if (first == second) continue;

        if (!second->collider.isCollidable) continue;

        if (first->objPtr->position.x + first->collider.collider.x +
                first->collider.collider.width <
            second->objPtr->position.x + MAX_COLL_OFFSET)
            break;

        if (CheckIfCollide(first, second)) return true;
    }
    return false;
}

objWrap *FindCollisionPos(objTracker *tracker, Vector2 pos) {
    if (tracker->objListLen < 1) return NULL;

    for (unsigned int j = 0; j < tracker->objListLen; j++) {
        objWrap *second = tracker->objList[j];
        if (tracker->objList[j] == NULL) continue;

        if (!second->collider.isCollidable) continue;

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
    unsigned long i        = 1;
    unsigned long gotToPos = 0;
    while (i < tracker->objListLen) {
        objWrap *prev    = tracker->objList[i - 1];
        objWrap *current = tracker->objList[i];
        if (!prev || !current) {
            fatal(EINVAL, 0, "Got a null while sorting");
            break;
        }

        if (!prev->objPtr || !current->objPtr) {
            fatal(EINVAL, 0, "Got a null objPtr while sorting");
            break;
        }

        if (current->objPtr->position.x >= prev->objPtr->position.x) {
            if (i > gotToPos) {
                gotToPos = i;
            } else
                i = gotToPos;
            i++;
            continue;
        }
        tracker->objList[i]     = prev;
        tracker->objList[i - 1] = current;
        if (i > 1) i--;
    }
}

void ApplyMassBasedRandRotation(objWrap *wrap) {

    float massMod   = 1 / wrap->collider.mass;
    float randomMod = (float)(GetRandomValue(-1000, 1000)) / 1000;

    wrap->objPtr->rotateSpeed = 2 * randomMod * massMod;
}

void Bounce(objTracker *tracker, objWrap *first, objWrap *second) {
    UNUSED(tracker);

    Vector2 pos_delta =
        VecSubVec(second->objPtr->position, first->objPtr->position);
    Vector2 speed_delta =
        VecSubVec(second->objPtr->speed, first->objPtr->speed);

    // If the signs match, that means that they're not going towards eachother,
    // thus no point to bounce them
    if (fGetSign(pos_delta.x) == fGetSign(speed_delta.x) &&
        fGetSign(pos_delta.y) == fGetSign(speed_delta.y))
        return;

    {

        // I wasted 24 fucking hours trying to make this work
        float Sa = first->objPtr->speed.x;
        float Sb = second->objPtr->speed.x;

        float ma =
            ClampFloat(first->collider.mass, 1, 1024) * BOUNCE_BACK_FACTOR;
        float mb =
            ClampFloat(second->collider.mass, 1, 1024) * BOUNCE_BACK_FACTOR;

        first->objPtr->speed.x  = Sa + (Sb - Sa) / ma;
        second->objPtr->speed.x = Sb + (Sa - Sb) / mb;

        LOG(TRACE, "MASSES:\n A == %f\n B == %f", ma, mb);
        LOG(TRACE, "SPEEDS BEFORE:\n First == %f\n Second == %f", Sa, Sb);
        LOG(TRACE,
            "SPEEDS AFTER:\n First == %f\n Second == %f",
            first->objPtr->speed.x,
            second->objPtr->speed.x);
    }

    {
        float Sa = first->objPtr->speed.y;
        float Sb = second->objPtr->speed.y;

        float ma =
            ClampFloat(first->collider.mass, 1, 1024) * BOUNCE_BACK_FACTOR;
        float mb =
            ClampFloat(second->collider.mass, 1, 1024) * BOUNCE_BACK_FACTOR;

        first->objPtr->speed.y  = Sa + (Sb - Sa) / ma;
        second->objPtr->speed.y = Sb + (Sa - Sb) / mb;

        LOG(TRACE, "MASSES:\n A == %f\n B == %f", ma, mb);
        LOG(TRACE, "SPEEDS BEFORE:\n First == %f\n Second == %f", Sa, Sb);
        LOG(TRACE,
            "SPEEDS AFTER:\n First == %f\n Second == %f",
            first->objPtr->speed.y,
            second->objPtr->speed.y);
    }
    FixClipping(first, second);
}

void FixClipping(objWrap *first, objWrap *second) {
    Vector2 pos_delta =
        VecSubVec(second->objPtr->position, first->objPtr->position);

    if (fabsf(fCutOff(pos_delta.x, 0)) >= fabsf(fCutOff(pos_delta.y, 0))) {
        const objWrap *left =
            first->objPtr->position.x < second->objPtr->position.x ? first
                                                                   : second;
        const objWrap *right = left == first ? second : first;

        left->objPtr->speed.x -= 1;
        right->objPtr->speed.x += 1;
    }
    if (fabsf(fCutOff(pos_delta.x, 0)) <= fabsf(fCutOff(pos_delta.y, 0))) {
        const objWrap *above =
            first->objPtr->position.y < second->objPtr->position.y ? first
                                                                   : second;
        const objWrap *below = above == first ? second : first;

        above->objPtr->speed.y -= 5;
        below->objPtr->speed.y += 5;
    }
}

bool CheckIfCollide(objWrap *first, objWrap *second) {
    // This abomination will make the logic actually readable
    // TODO: Rewrite this with vector-math lib
    Vector2 firstStart = {
        first->objPtr->position.x +     // leftmost point of the collider
            first->collider.collider.x, // in the abs coord system

        first->objPtr->position.y +    // topmost point of the collider
            first->collider.collider.y // in the abs coord system
    };
    Vector2 firstEnd    = { firstStart.x + // The rightmost point
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
    //  +--------+ <- first object
    //  |   +----|---+ <- second object
    //  |   |    |   |
    //  |   |    |   |
    //  +---|----+   |
    //      |        |
    //      +--------+
    // In that example  second object overlaps the first one from the
    // bottom-right
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
    bool collide_x =
        (firstStart.x <= secondEnd.x && secondStart.x <= firstEnd.x) ||
        (firstStart.x >= secondEnd.x && secondStart.x >= firstEnd.x);
    bool collide_y =
        (firstStart.y <= secondEnd.y && secondStart.y <= firstEnd.y) ||
        (firstStart.y >= secondEnd.y && secondStart.y >= firstEnd.y);

    if (collide_x && collide_y) return true;

    return false;
}

void GetShot(objTracker *tracker, objWrap *projectile, objWrap *victim) {

    projectile->request = DELETE;
    if (victim->objectType == PROJECTILE) victim->request = DELETE;

    if (victim->objectType == ASTEROID) {
        victim->livesLeft--;
        victim->request = SEPARATE;
        tracker->playerScore += 1 * CUR_DIFFICULTY;
    }
}

void FastFindCollisions(objTracker *tracker, unsigned long index) {
    objWrap *current = tracker->objList[index];
    if (tracker->objListLen < 2) return;
    if (!current || !current->objPtr || current->request == DELETE) return;

    objWrap *next;
    for (unsigned long j = index + 1; j < tracker->objListLen; j++) {
        next = tracker->objList[j];

        if (next == NULL || current == next || next->request != UPDATE ||
            !next->collider.isCollidable)
            continue;

        pthread_mutex_lock(&next->mutex);
        if (current->objPtr->position.x + current->collider.collider.x +
                current->collider.collider.width <
            next->objPtr->position.x + MAX_COLL_OFFSET) {
            pthread_mutex_unlock(&next->mutex);
            break;
        }

        if (!CheckIfCollide(current, next)) {
            pthread_mutex_unlock(&next->mutex);
            continue;
        }

        if (current->objectType == next->objectType &&
            current->objectType == PROJECTILE) {
            pthread_mutex_unlock(&next->mutex);
            continue;
        }

        if (current->objectType > next->objectType) {
            current->collider.ActionOnCollision(tracker, current, next);
            pthread_mutex_unlock(&next->mutex);
            continue;
        }
        next->collider.ActionOnCollision(tracker, next, current);
        pthread_mutex_unlock(&next->mutex);
    }
    return;
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

    float leftMostPoint   = 10000;
    float rightMostPoint  = -10000;
    float topMostPoint    = 10000;
    float bottomMostPoint = -10000;

    for (unsigned int i = 0; i < wrap->objPtr->shape.arrayLength; i++) {
        leftMostPoint   = wrap->objPtr->shape.points[i].x < leftMostPoint
                              ? wrap->objPtr->shape.points[i].x
                              : leftMostPoint;
        rightMostPoint  = wrap->objPtr->shape.points[i].x > rightMostPoint
                              ? wrap->objPtr->shape.points[i].x
                              : rightMostPoint;
        topMostPoint    = wrap->objPtr->shape.points[i].y < topMostPoint
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

void PlayerCollision(objTracker *tracker, objWrap *player, objWrap *offender) {
    player->livesLeft--;

    if (!player->livesLeft) {
        GAME_STATE = GAME_OVER;
        return;
    }

    if (offender->objectType == PROJECTILE) offender->request = DELETE;
    if (offender->objectType == ASTEROID) Bounce(tracker, player, offender);

    return;
}
