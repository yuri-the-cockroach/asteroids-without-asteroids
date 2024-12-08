#ifndef BENCHMARKING_H_
#define BENCHMARKING_H_
#include "structs.h"
#include "logger.h"
#include "autils.h"

int BenchStart(long *timerStart);

int BenchEnd(long *timerStart, const char *restrict name);

#endif // BENCHMARKING_H_
