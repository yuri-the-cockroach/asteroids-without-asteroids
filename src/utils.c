#include "utils.h"
#include <raylib.h>

float GetRandomFloat(float min, float max) {
    return (float)GetRandomValue((int)(min * 1000), (int)(max * 1000)) / 1000;
}

// Clap the value D from min to max (I hate this code, but it's good in
// assembly)
float clamp(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}
