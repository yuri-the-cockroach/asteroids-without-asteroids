#ifndef ASTEROIDSUTILS_H_
#define ASTEROIDSUTILS_H_

#include "logger.h"
#include "structs.h"
#include "tracingtools.c"
#include <assert.h>
#include <sys/stat.h>

#define UNUSED(x) (void)(x)
#define DEBUG(a)

#ifdef DEBUGGING
    #pragma GCC diagnostic ignored "-Wmacro-redefined"
    #define DEBUG(a) a
#endif // DEBUGGING

// Cutoff decimal digits after `n` from a float,
// Where `n` is a power of 10,
// i.e. `fCutOff(10.4242f, 2)` -> `10.42f`
float fCutOff(float f, int n);

// Clap float from min to max
// I hate this code
// but it's good in assembly
float ClampFloat(float d, float min, float max);

// returns you the sign of the value.
// 1 if positive
// -1 if negative
int fGetSign(float f);

// returns you the sign of the value.
// 1 if positive
// -1 if negative
int GetSign(int i);

// Clap int from min to max
// I hate this code,
// but it's good in assembly
int ClampInt(int d, int min, int max);

// Gets you a random float between min and max
// Wrapper for raylib GetRandomValue
float GetRandomf(float min, float max);

// Rollover float, if it gets over max or under min
float fRollOver(float d, float min, float max);

// Get current time in milliseconds
long GetTimeMicS(void);

long GetTimeMicS(void) {
    struct timeval tv = { 0, 0 };
    gettimeofday(&tv, 0);
    return tv.tv_sec * (long)1e6 + tv.tv_usec;
}

// Cleanup memory find all NULLs in memory
// remove them and rearenge memory
void CleanupMemory(objTracker *tracker);

// Rollover int, if it gets over max or under min
int RollOver(int d, int min, int max);

int fGetSign(float f) {
    int *ptr = (int *)(void *)&f;
    return *ptr >> 31 | 1;
}

int GetSign(int i) { return i >> 31 | 1; }

float GetRandomf(float min, float max) {
    return (float)(GetRandomValue((int)(min * 1000), (int)(max * 1000))) / 1000;
}

float fRollOver(float d, float min, float max) {
    const float t = d < min ? max : d;
    return t > max ? min : t;
}

float fCutOff(float f, int n) {
    if (n == 0) return (float)(int)f;
    const float mult = powf(10.f, (float)n);
    const int temp   = (int)(f * mult);
    return (float)temp / mult;
}

int RollOver(int d, int min, int max) {
    const int t = d < min ? max : d;
    return t > max ? min : t;
}

float ClampFloat(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

int ClampInt(int d, int min, int max) {
    const int t = d < min ? min : d;
    return t > max ? max : t;
}

FILE *CreateLogFile(char *restrict file_name_ptr);

int GetStartUpArguments(int argc, char **argv);

// Initializes needed config
void RunConfig(void);

void InitRaylib(void);

// Cleansup everthing created with `RunConfig()`
void RunCleanup(void);

#endif // UTILS_H_
