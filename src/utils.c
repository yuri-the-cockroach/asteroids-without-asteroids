#include "utils.h"
#include <raylib.h>
#include <sys/time.h>

float GetRandomFloat(float min, float max) {
    return (float)(GetRandomValue((int)(min * 1000), (int)(max * 1000))) / 1000;
}

// Clap the value D from min to max (I hate this code, but it's good in
// assembly)
float clampFloat(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

int clampInt(int d, int min, int max) {
    const int t = d < min ? min : d;
    return t > max ? max : t;
}

long GetTimeMS(void) {
    struct timeval tv;

    gettimeofday( &tv, 0);
    return tv.tv_usec / 1000;
}



const char * Format(const char *restrict format,...);
