#include "benchmarking.h"
#include "asteroidsutils.h"
#include "structs.h"

int BenchStart(long *timerStart) {
    if ( !BENCHMARKING ) return 0;
    *timerStart = GetTimeMicS();
    return 0;
}

int BenchEnd(long *timerStart, const char *restrict name) {
    if ( !BENCHMARKING ) return 0;
    LOG(BENCH, "%s took %ldus", name, GetTimeMicS() - *timerStart);
    return 0;
}
