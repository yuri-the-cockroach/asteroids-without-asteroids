#ifndef COLLISION_H_
#define COLLISION_H_

#include "logger.h"
#include "structs.h"
#include <immintrin.h>

#define BOUCE_CONSTANT 100

static const __m256i_u shiftByVec = {
    (31l << 32) + 31, (31l << 32) + 31, (31l << 32) + 31, (31l << 32) + 31
};

static const __m256i bitMaskVec = {
    (1l << 32) + 1, (1l << 32) + 1, (1l << 32) + 1, (1l << 32) + 1
};

void *EasyThreadAVX(void *arg);

// Thread function that looks for collisions
void *EasyThread(void *arg);

// Run all threads for one cycle
int RunThreads(struct mt_data_wrap *dataWrap);

// Make sure that all threads are finished
int CollectThreads(struct mt_data_wrap *dataWrap);

// Cleanup after MT functions
int MTCleanupAndFree(struct mt_data_wrap *dataWrap);

// Create threads and prepare them for `RunThreads` func
// return `struct mt_data_wrap` they are managed by
struct mt_data_wrap *InitMT(objTracker *tracker);

// Check anything collides with this object
// Return either TRUE or FALSE
bool FindAnyCollision(objTracker *tracker, objWrap *first);

// Sort the list by X coordinate
void SortListByX(objTracker *tracker);

// An algorithm that shoud in theory be a lot faster
void FastFindCollisions(objTracker *tracker, unsigned long index);

// Find an object that ocupies pos
objWrap *FindCollisionPos(objTracker *tracker, Vector2 pos);

// Changes object's rotation taking it's mass into account
void ApplyMassBasedRandRotation(objWrap *wrap);

// Check if two objects collide
bool CheckIfCollide(objWrap *first, objWrap *second);

// Collision response function
void Bounce(objTracker *tracker, objWrap *first, objWrap *second);

// If object callided with a projectile
void GetShot(objTracker *tracker, objWrap *first, objWrap *second);

collider InitCollider(float sizeMult,
                      void (*ActionOnCollision)(objTracker *tracker,
                                                objWrap *first,
                                                objWrap *second));

int UpdateCollider(objWrap *wrap);

void PlayerCollision(objTracker *tracker, objWrap *player, objWrap *offender);

#endif // COLLISION_H_
