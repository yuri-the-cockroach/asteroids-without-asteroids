#ifndef UTILS_H_
#define UTILS_H_

#define UNUSED(x) (void)(x)

float clampFloat(float d, float min, float max);
int clampInt(int d, int min, int max);
float GetRandomFloat(float min, float max);
long GetTimeMS(void);

#endif // UTILS_H_
