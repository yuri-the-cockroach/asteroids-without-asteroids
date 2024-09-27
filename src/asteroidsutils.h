#ifndef ASTEROIDSUTILS_H_
#define ASTEROIDSUTILS_H_

#include "structs.h"


#define UNUSED(x) (void)(x)

// Clap float from min to max
// I hate this code
// but it's good in assembly
float ClampFloat(float d, float min, float max);

// Clap int from min to max
// I hate this code,
// but it's good in assembly
int ClampInt(int d, int min, int max);

// Gets you a random float between min and max
// Wrapper for raylib GetRandomValue
float GetRandomFloat(float min, float max);

// Get current time in milliseconds
long GetTimeMS(void);

// Cleanup memory find all NULLs in memory
// remove them and rearenge memory
void CleanupMemory(ObjectTracker *tracker);

// Rollover int, if it gets over max or under min
int RollOverInt(int d, int min, int max);

// Rollover float, if it gets over max or under min
float RollOverFloat(float d, float min, float max);
#endif // UTILS_H_
