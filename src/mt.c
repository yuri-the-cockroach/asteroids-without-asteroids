#include "mt.h"
#include "asteroid.h"
#include "collision.h"
#include "logger.h"
#include "objecthandler.h"
#include "render.h"
#include <pthread.h>

#include <errno.h>

// FIXME This function is WIP and BROKEN and is will not perform as expected, so
// it should not be used for now

// void *EasyThreadAVX(void *arg) {
//     objWrap *current, *next;
//     __m256 first_x_st = { 0 }, first_x_en = { 0 }, second_x_en = { 0 },
//            second_x_st = { 0 }, first_y_st = { 0 }, second_y_st = { 0 },
//            first_y_en = { 0 }, second_y_en = { 0 };
//     __m256i y_ss_fe = { 0 }, y_fs_se = { 0 }, x_ss_fe = { 0 }, x_fs_se = { 0
//     }; int *i32_ptr_y_ss_fe, *i32_ptr_y_fs_se, *i32_ptr_x_ss_fe,
//     *i32_ptr_x_fs_se;

//     struct mt_init_struct *init = (struct mt_init_struct *)arg;
//     init->KeepRunning           = true;
//     objTracker *tracker         = init->tracker;
//     objWrap **objList           = tracker->objList;

//     unsigned long size = tracker->objListLen;
//     pthread_mutex_init(&init->threadMutex, NULL);
//     pthread_cond_init(&init->threadWakeupSignal, NULL);
//     pthread_mutex_lock(&init->threadMutex);
//     float f_x_st, f_x_en, f_y_st, f_y_en, s_x_st, s_x_en, s_y_st, s_y_en;
//     pthread_cond_wait(&init->threadWakeupSignal, &init->threadMutex);
//     do {
//         size = tracker->objListLen;
//         for (unsigned int i = init->id; i < tracker->objListLen;
//              i += N_CPU_THREADS) {

//             current = tracker->objList[i];

//             f_x_st = current->objPtr->position.x +
//             current->collider.collider.x; f_x_en = f_x_st +
//             current->collider.collider.width;

//             f_y_st = current->objPtr->position.y +
//             current->collider.collider.y; f_y_en = f_y_st +
//             current->collider.collider.height;

//             first_x_st = (__m256){ f_x_st, f_x_st, f_x_st, f_x_st,
//                                    f_x_st, f_x_st, f_x_st, f_x_st };

//             first_x_en = (__m256){ f_x_en, f_x_en, f_x_en, f_x_en,
//                                    f_x_en, f_x_en, f_x_en, f_x_en };

//             first_y_st = (__m256){ f_y_st, f_y_st, f_y_st, f_y_st,
//                                    f_y_st, f_y_st, f_y_st, f_y_st };

//             first_y_en = (__m256){ f_y_en, f_y_en, f_y_en, f_y_en,
//                                    f_y_en, f_y_en, f_y_en, f_y_en };

//             if (i + 8 > size) {
//                 for (unsigned long n = i + 8 - size; n > 0; n--) {
//                     first_x_st[8 - n] = 0;
//                     first_x_en[8 - n] = 0;
//                     first_y_st[8 - n] = 0;
//                     first_y_en[8 - n] = 0;
//                 }
//             }

//             for (unsigned int j = i + 1; j < tracker->objListLen; j += 8) {

//                 if (objList[j] == NULL ||
//                     first_x_en[0] <= objList[j]->objPtr->position.x +
//                                          objList[j]->collider.collider.x)
//                     break;

//                 // LOG(INFO, "size: %d, j: %d", size, j);
//                 for (unsigned long n = j; n < size && n - j < 8; n++) {
//                     if (objList[n] == NULL || objList[n]->request != UPDATE
//                     ||
//                         !objList[n]->collider.isCollidable)
//                         continue;
//                     s_x_st = objList[n]->objPtr->position.x +
//                              objList[n]->collider.collider.x;
//                     s_x_en = s_x_st + objList[n]->collider.collider.width;

//                     s_y_st = objList[n]->objPtr->position.y +
//                              objList[n]->collider.collider.y;
//                     s_y_en = s_y_st + objList[n]->collider.collider.height;

//                     second_x_st[n - j] = s_x_st;
//                     second_x_en[n - j] = s_x_en;
//                     second_y_st[n - j] = s_y_st;
//                     second_y_en[n - j] = s_y_en;
//                 }

//                 if (i + 8 > size) {
//                     for (unsigned long n = j + 8 - size; n > 0; n--) {
//                         first_x_st[8 - n]  = 0;
//                         first_x_en[8 - n]  = 0;
//                         first_y_st[8 - n]  = 0;
//                         first_y_en[8 - n]  = 0;
//                         second_x_st[8 - n] = 0;
//                         second_x_en[8 - n] = 0;
//                         second_y_st[8 - n] = 0;
//                         second_y_en[8 - n] = 0;
//                     }
//                 }

//                 x_fs_se = _mm256_and_si256(
//                     _mm256_srlv_epi32(
//                         (__m256i)_mm256_sub_ps(first_x_st, second_x_en),
//                         shiftByVec),
//                     bitMaskVec); // second x - first x to make sure that
//                                  // there's actually a collision

//                 x_ss_fe = _mm256_and_si256(
//                     _mm256_srlv_epi32(
//                         (__m256i)_mm256_sub_ps(second_x_st, first_x_en),
//                         shiftByVec),
//                     bitMaskVec); // second x - first x to make sure that
//                                  // there's actually a collision
//                 y_fs_se = _mm256_and_si256(
//                     _mm256_srlv_epi32(
//                         (__m256i)_mm256_sub_ps(first_y_st, second_y_en),
//                         shiftByVec),
//                     bitMaskVec); // first start - second end

//                 y_ss_fe = _mm256_and_si256(
//                     _mm256_srlv_epi32(
//                         (__m256i)_mm256_sub_ps(second_y_st, first_y_en),
//                         shiftByVec),
//                     bitMaskVec); // second start - first end

//                 i32_ptr_x_ss_fe = (int *)(void *)&x_ss_fe;
//                 i32_ptr_x_fs_se = (int *)(void *)&x_fs_se;
//                 i32_ptr_y_ss_fe = (int *)(void *)&y_ss_fe;
//                 i32_ptr_y_fs_se = (int *)(void *)&y_fs_se;

//                 for (unsigned int n = 0; n < 8 && j + n < size; n++) {
//                     next = objList[j + n];

//                     if (current->objectType == next->objectType &&
//                         current->objectType == PROJECTILE)
//                         continue;

//                     if (!(i32_ptr_x_fs_se[n] & i32_ptr_x_ss_fe[n] &
//                           i32_ptr_y_fs_se[n] & i32_ptr_y_ss_fe[n]))
//                         continue;

//                     if (current->objectType > next->objectType) {
//                         current->collider.ActionOnCollision(
//                             tracker, current, next);
//                         continue;
//                     }
//                     next->collider.ActionOnCollision(tracker, next, current);
//                 }
//             }
//         }
//         pthread_cond_wait(&init->threadWakeupSignal, &init->threadMutex);
//     } while (init->KeepRunning);
//     return NULL;
// }

// What it does:
// * Ask all the threads to stop
// * Wait for them all join
// * Cleanup all the allocations made for MT
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

int CollectThreads(struct mt_data_wrap *dataWrap) {
    for (unsigned int i = 0; i < N_CPU_THREADS; i++) {
        pthread_mutex_lock(&dataWrap->initStructList[i].threadMutex);
        pthread_mutex_unlock(&dataWrap->initStructList[i].threadMutex);
    }
    return 0;
}

void *EasyThread(void *arg) {
    objWrap *current;

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

            if (!current) continue;
            pthread_mutex_lock(&current->mutex);
            // If it is somehow NULL, then cleanup:
            // Check if there's any elements ahead at all. If not just
            // decrement the length of the list.

            switch (current->request) {
            case UPDATE: // Call the updater function of the element
            {
                FastFindCollisions(tracker, i);
                if (current->isRotatableByGame)
                    RotateObject(current, (current->objPtr->rotateSpeed));
                break;
            }

            case IGNORE:
                break;

            default:
                break;
            }
            pthread_mutex_unlock(&current->mutex);
        }
        pthread_cond_wait(&init->threadWakeupSignal, &init->threadMutex);
    } while (init->KeepRunning);
    return NULL;
}

int RunThreads(struct mt_data_wrap *dataWrap) {
    for (unsigned int i = 0; i < N_CPU_THREADS; i++) {
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

    LOG(DEBUG, "Initializing %d threads", N_CPU_THREADS);
    pthread_mutexattr_t attr = { 0 };

    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    for (unsigned int i = 0; i < N_CPU_THREADS; i++) {
        dataWrap->initStructList[i] =
            (struct mt_init_struct){ .id          = i,
                                     .tracker     = tracker,
                                     .threadMutex = PTHREAD_MUTEX_INITIALIZER,
                                     .KeepRunning = true,
                                     .threadWakeupSignal =
                                         PTHREAD_COND_INITIALIZER };
        pthread_mutex_init(&dataWrap->initStructList[i].threadMutex, &attr);
        pthread_cond_init(&dataWrap->initStructList[i].threadWakeupSignal,
                          NULL);
        pthread_create(&dataWrap->threads[i],
                       NULL,
                       EasyThread,
                       &dataWrap->initStructList[i]);
        LOG(DEBUG, "Initialized thread %d", i);
    }

    return dataWrap;
}
