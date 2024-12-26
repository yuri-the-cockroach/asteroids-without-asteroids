// system includes
#include <errno.h>
#include <immintrin.h>
#include <pthread.h>
#include <stdlib.h>

// local includes
#include "autils.h"
#include "collision.h"
#include "logger.h"
#include "structs.h"

// NOTE This function is WIP and is may not perform as expected, so it should
// not be used for now
void *EasyThreadAVX(void *arg) {
    objWrap *current, *next;
    __m256 first_x_st = { 0 }, first_x_en = { 0 }, second_x_en = { 0 },
           second_x_st = { 0 }, first_y_st = { 0 }, second_y_st = { 0 },
           first_y_en = { 0 }, second_y_en = { 0 };
    __m256i y_ss_fe = { 0 }, y_fs_se = { 0 }, x_ss_fe = { 0 }, x_fs_se = { 0 };
    int *i32_ptr_y_ss_fe, *i32_ptr_y_fs_se, *i32_ptr_x_ss_fe, *i32_ptr_x_fs_se;

    struct mt_init_struct *init = (struct mt_init_struct *)arg;
    init->KeepRunning           = true;
    objTracker *tracker         = init->tracker;
    objWrap **objList           = tracker->objList;

    unsigned long size = tracker->objListLen;
    pthread_mutex_init(&init->threadMutex, NULL);
    pthread_cond_init(&init->threadWakeupSignal, NULL);
    pthread_mutex_lock(&init->threadMutex);
    float f_x_st, f_x_en, f_y_st, f_y_en, s_x_st, s_x_en, s_y_st, s_y_en;
    pthread_cond_wait(&init->threadWakeupSignal, &init->threadMutex);

    do {
        size = tracker->objListLen;
        for (unsigned int i = init->id; i < tracker->objListLen;
             i += N_CPU_THREADS) {
            current = tracker->objList[i];

            f_x_st = current->objPtr->position.x + current->collider.collider.x;
            f_x_en = f_x_st + current->collider.collider.width;

            f_y_st = current->objPtr->position.y + current->collider.collider.y;
            f_y_en = f_y_st + current->collider.collider.height;

            first_x_st = (__m256){ f_x_st, f_x_st, f_x_st, f_x_st,
                                   f_x_st, f_x_st, f_x_st, f_x_st };

            first_x_en = (__m256){ f_x_en, f_x_en, f_x_en, f_x_en,
                                   f_x_en, f_x_en, f_x_en, f_x_en };

            first_y_st = (__m256){ f_y_st, f_y_st, f_y_st, f_y_st,
                                   f_y_st, f_y_st, f_y_st, f_y_st };

            first_y_en = (__m256){ f_y_en, f_y_en, f_y_en, f_y_en,
                                   f_y_en, f_y_en, f_y_en, f_y_en };

            if (i + 8 > size) {
                for (unsigned long n = i + 8 - size; n > 0; n--) {
                    first_x_st[8 - n] = 0;
                    first_x_en[8 - n] = 0;
                    first_y_st[8 - n] = 0;
                    first_y_en[8 - n] = 0;
                }
            }

            for (unsigned int j = i + 1; j < tracker->objListLen; j += 8) {

                if (objList[j] == NULL ||
                    first_x_en[0] <= objList[j]->objPtr->position.x +
                                         objList[j]->collider.collider.x)
                    break;

                // LOG(INFO, "size: %d, j: %d", size, j);
                for (unsigned long n = j; n < size && n - j < 8; n++) {
                    if (objList[n] == NULL || objList[n]->request != UPDATE ||
                        !objList[n]->collider.isCollidable)
                        continue;
                    s_x_st = objList[n]->objPtr->position.x +
                             objList[n]->collider.collider.x;
                    s_x_en = s_x_st + objList[n]->collider.collider.width;

                    s_y_st = objList[n]->objPtr->position.y +
                             objList[n]->collider.collider.y;
                    s_y_en = s_y_st + objList[n]->collider.collider.height;

                    second_x_st[n - j] = s_x_st;
                    second_x_en[n - j] = s_x_en;
                    second_y_st[n - j] = s_y_st;
                    second_y_en[n - j] = s_y_en;
                }

                if (i + 8 > size) {
                    for (unsigned long n = j + 8 - size; n > 0; n--) {
                        first_x_st[8 - n]  = 0;
                        first_x_en[8 - n]  = 0;
                        first_y_st[8 - n]  = 0;
                        first_y_en[8 - n]  = 0;
                        second_x_st[8 - n] = 0;
                        second_x_en[8 - n] = 0;
                        second_y_st[8 - n] = 0;
                        second_y_en[8 - n] = 0;
                    }
                }

                x_fs_se = _mm256_and_si256(
                    _mm256_srlv_epi32(
                        (__m256i)_mm256_sub_ps(first_x_st, second_x_en),
                        shiftByVec),
                    bitMaskVec); // second x - first x to make sure that there's
                                 // actually a collision

                x_ss_fe = _mm256_and_si256(
                    _mm256_srlv_epi32(
                        (__m256i)_mm256_sub_ps(second_x_st, first_x_en),
                        shiftByVec),
                    bitMaskVec); // second x - first x to make sure that there's
                                 // actually a collision
                y_fs_se = _mm256_and_si256(
                    _mm256_srlv_epi32(
                        (__m256i)_mm256_sub_ps(first_y_st, second_y_en),
                        shiftByVec),
                    bitMaskVec); // first start - second end

                y_ss_fe = _mm256_and_si256(
                    _mm256_srlv_epi32(
                        (__m256i)_mm256_sub_ps(second_y_st, first_y_en),
                        shiftByVec),
                    bitMaskVec); // second start - first end

                i32_ptr_x_ss_fe = (int *)(void *)&x_ss_fe;
                i32_ptr_x_fs_se = (int *)(void *)&x_fs_se;
                i32_ptr_y_ss_fe = (int *)(void *)&y_ss_fe;
                i32_ptr_y_fs_se = (int *)(void *)&y_fs_se;

                for (unsigned int n = 0; n < 8 && j + n < size; n++) {
                    next = objList[j + n];

                    if (current->objectType == next->objectType &&
                        current->objectType == PROJECTILE)
                        continue;

                    if (!(i32_ptr_x_fs_se[n] & i32_ptr_x_ss_fe[n] &
                          i32_ptr_y_fs_se[n] & i32_ptr_y_ss_fe[n]))
                        continue;

                    if (current->objectType > next->objectType) {
                        current->collider.ActionOnCollision(
                            tracker, current, next);
                        continue;
                    }
                    next->collider.ActionOnCollision(tracker, next, current);
                }
            }
        }
        pthread_cond_wait(&init->threadWakeupSignal, &init->threadMutex);
    } while (init->KeepRunning);
    return NULL;
}

void *EasyThread(void *arg) {
    objWrap *current, *next;

    struct mt_init_struct *init = (struct mt_init_struct *)arg;
    init->KeepRunning           = true;
    objTracker *tracker         = init->tracker;

    pthread_mutex_init(&init->threadMutex, NULL);
    pthread_cond_init(&init->threadWakeupSignal, NULL);
    pthread_mutex_lock(&init->threadMutex);

    pthread_cond_wait(&init->threadWakeupSignal, &init->threadMutex);
    do {
        for (unsigned int i = init->id; i < tracker->objListLen;
             i += N_CPU_THREADS) {
            current = tracker->objList[i];
            for (unsigned int j = i + 1; j < tracker->objListLen; j++) {
                next = tracker->objList[j];

                if (tracker->objList[j] == NULL || current == next ||
                    next->request != UPDATE || !next->collider.isCollidable)
                    continue;

                if (current->objectType == next->objectType &&
                    current->objectType == PROJECTILE)
                    continue;

                if (current->collider.collider.x +
                        current->collider.collider.width <=
                    next->collider.collider.x)
                    break;

                if (!CheckIfCollide(current, next)) continue;

                if (current->objectType > next->objectType) {
                    current->collider.ActionOnCollision(tracker, current, next);
                    continue;
                }

                next->collider.ActionOnCollision(tracker, next, current);
            }
        }
        pthread_cond_wait(&init->threadWakeupSignal, &init->threadMutex);
    } while (init->KeepRunning);
    return NULL;
}

int CollectThreads(struct mt_data_wrap *dataWrap) {
    for (unsigned int i = 0; i < N_CPU_THREADS; i++) {
        pthread_mutex_lock(&dataWrap->initStructList[i].threadMutex);
        pthread_mutex_unlock(&dataWrap->initStructList[i].threadMutex);
    }
    return 0;
}

int MTCleanupAndFree(struct mt_data_wrap *dataWrap) {
    if (!dataWrap) {
        LOG(DEBUG, "%", "Null pointer was passed to be freed");
        return 0;
    }

    for (unsigned int i = 0; i < N_CPU_THREADS; i++) {
        dataWrap->initStructList[i].KeepRunning = false;
        pthread_cond_signal(&dataWrap->initStructList[i].threadWakeupSignal);
    }

    for (unsigned int i = 0; i < N_CPU_THREADS; i++) {
        pthread_join(dataWrap->threads[i], NULL);
    }

    free(dataWrap->initStructList);
    free(dataWrap->threads);
    free(dataWrap);
    return 0;
}

int RunThreads(struct mt_data_wrap *dataWrap) {
    for (unsigned int i = 0; i < N_CPU_THREADS; i++) {
        pthread_mutex_unlock(&dataWrap->initStructList[i].threadMutex);
        int ret = pthread_cond_signal(
            &dataWrap->initStructList[i].threadWakeupSignal);
        errno = ret ? ret : errno;
    }
    return errno;
}

struct mt_data_wrap *InitMT(objTracker *tracker) {

    struct mt_data_wrap *dataWrap = calloc(1, sizeof(struct mt_data_wrap));
    *dataWrap                     = (struct mt_data_wrap){
                            .threads = calloc((unsigned long)N_CPU_THREADS, sizeof(pthread_t)),
                            .initStructList =
            calloc((unsigned long)N_CPU_THREADS, sizeof(struct mt_init_struct)),
    };

    for (unsigned int i = 0; i < N_CPU_THREADS; i++) {
        dataWrap->initStructList[i] =
            (struct mt_init_struct){ .id = i, .tracker = tracker };
        pthread_create(&dataWrap->threads[i],
                       NULL,
                       EasyThread,
                       &dataWrap->initStructList[i]);
    }

    return dataWrap;
}

bool FindAnyCollision(objTracker *tracker, objWrap *first) {
    if (tracker->objListLen < 2) return false;

    if (!first) return false;

    for (unsigned int j = 0; j < tracker->objListLen; j++) {
        objWrap *second = tracker->objList[j];
        if (tracker->objList[j] == NULL) continue;

        if (first == second) continue;

        if (!second->collider.isCollidable) continue;

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

    objWrap *left, *right;
    objWrap *above, *below;

    float frameTime = GetFrameTime();

    // If A.x < B.x then A is left of B
    left  = first->objPtr->position.x < second->objPtr->position.x
                ? first
                : second;                   // Find the left one
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

        first->objPtr->speed.x  = Sa + (Sb - Sa) / ma;
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

        first->objPtr->speed.y  = Sa + (Sb - Sa) / ma;
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

    if (!current) return;

    if (current->request == DELETE) return;

    objWrap *next;

    for (unsigned long j = index + 1; j < tracker->objListLen; j++) {
        next = tracker->objList[j];

        if (tracker->objList[j] == NULL || current == next ||
            next->request != UPDATE || !next->collider.isCollidable)
            continue;

        if (current->objPtr->position.x + current->collider.collider.x +
                current->collider.collider.width <=
            next->objPtr->position.x + next->collider.collider.x)
            break;

        if (!CheckIfCollide(current, next)) continue;

        if (current->objectType == next->objectType &&
            current->objectType == PROJECTILE)
            continue;

        if (current->objectType > next->objectType) {
            current->collider.ActionOnCollision(tracker, current, next);
            continue;
        }
        next->collider.ActionOnCollision(tracker, next, current);
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
