#include "benchmarking.h"
#include "autils.h"
#include "structs.h"

int BenchStart(long *timerStart) {
    *timerStart = GetTimeMicS();
    return 0;
}

int BenchEnd(long *timerStart, const char *restrict name) {
    LOG(BENCH, "%s took %ldus", name, GetTimeMicS() - *timerStart);
    return 0;
}
