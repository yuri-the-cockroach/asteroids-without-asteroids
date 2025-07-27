#ifndef COCK_MT_H_
#define COCK_MT_H_
#include "structs.h"
#include <immintrin.h>

static const __m256i_u shiftByVec = {(31l << 32) + 31, (31l << 32) + 31,
                                     (31l << 32) + 31, (31l << 32) + 31};

static const __m256i bitMaskVec = {(1l << 32) + 1, (1l << 32) + 1,
                                   (1l << 32) + 1, (1l << 32) + 1};

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

#endif // COCK_MT_H_
