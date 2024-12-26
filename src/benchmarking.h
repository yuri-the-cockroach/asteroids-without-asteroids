#ifndef BENCHMARKING_H_
#define BENCHMARKING_H_

extern long total_upd_time;

#define BENCH(what, total)              \
    ({                                  \
        long start = GetTimeMicS();     \
        what;                           \
        total += GetTimeMicS() - start; \
    })

int BenchLog(...);
int BenchStart(long *timerStart);

int BenchEnd(long *timerStart, const char *restrict name);

#endif // BENCHMARKING_H_
