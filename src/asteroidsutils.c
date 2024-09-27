#include "asteroidsutils.h"

float GetRandomFloat(float min, float max) {
    return (float)(GetRandomValue((int)(min * 1000), (int)(max * 1000))) / 1000;
}

// Rollover the number, if it gets over max or under min
float RollOverFloat(float d, float min, float max) {
    const float t = d < min ? max : d;
    return t > max ? min : t;
}

// Rollover the number, if it gets over max or under min
int RollOverInt(int d, int min, int max) {
    const int t = d < min ? max : d;
    return t > max ? min : t;
}

// Clap the value D from min to max (I hate this code, but it's good in
// assembly)
float ClampFloat(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}


int ClampInt(int d, int min, int max) {
    const int t = d < min ? min : d;
    return t > max ? max : t;
}

long GetTimeMS(void) {
    struct timeval tv = {0, 0};
    gettimeofday(&tv, 0);
    return tv.tv_sec * (long)1e6 + tv.tv_usec;
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

const char *Format(const char *restrict format, ...);
