#ifndef ASTEROIDSUTILS_H_
#define ASTEROIDSUTILS_H_

#include "structs.h"
#include "logger.h"
#include <time.h>
#include <sys/stat.h>

#define UNUSED(x) (void)(x)

#pragma GCC diagnostic ignored "-Wunused-macros"
#define BENCH(a, str) a

#ifdef BENCHMARKING

#pragma GCC diagnostic ignored "-Wmacro-redefined"
#define BENCH(a, str) \
{ \
    long start = 0; \
    BenchStart(&start);\
    a                   \
    BenchEnd(&start, str);\
}
#endif

#pragma GCC diagnostic ignored "-Wunused-macros"
#define DEBUG(a) a

#ifdef BENCHMARKING

#pragma GCC diagnostic ignored "-Wmacro-redefined"
#define DEBUG(a) \
a
#endif



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
float GetRandomFloat(float min, float max);

// Get current time in milliseconds
long GetTimeMicS(void);

// Cleanup memory find all NULLs in memory
// remove them and rearenge memory
void CleanupMemory(objTracker *tracker);

// Rollover int, if it gets over max or under min
int RollOverInt(int d, int min, int max);

// Rollover float, if it gets over max or under min
float RollOverFloat(float d, float min, float max);

int CreateLogFile(void);

int GetStartUpArguments(int argc, char **argv);

#endif // UTILS_H_
