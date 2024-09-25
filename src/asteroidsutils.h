#ifndef ASTEROIDSUTILS_H_
#define ASTEROIDSUTILS_H_

#include "structs.h"

#define UNUSED(x) (void)(x)

float ClampFloat(float d, float min, float max);
int ClampInt(int d, int min, int max);
float GetRandomFloat(float min, float max);
long GetTimeMS(void);
void CleanupMemory(ObjectTracker *tracker);

#endif // UTILS_H_
