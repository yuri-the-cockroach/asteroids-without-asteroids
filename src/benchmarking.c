#include "benchmarking.h"
#include "autils.h"
#include "structs.h"
#include <errno.h>

#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

int BenchLog(...) {

    if (!LOG_FILE_PTR) {
        LOG(FATAL, "Log file pointer is NULL!");
        errno = EFAULT;
        err(errno, "Benchmark log file pointer is NULL!");
    }

    char messageString[1024] = "";

    va_list argptr;

    va_start(argptr);

    const char *restrict format = va_arg(argptr, const char *restrict);
    vsnprintf(messageString, 1024, format, argptr);
    va_end(argptr);

    fprintf(LOG_FILE_PTR, "%s", messageString);
    fprintf(stderr, "%s", messageString);

    return 0;
}

int BenchStart(long *timerStart) {
    *timerStart = GetTimeMicS();
    return 0;
}

int BenchEnd(long *timerStart, const char *restrict name) {
    BenchLog("%s took %ldus", name, GetTimeMicS() - *timerStart);
    return 0;
}
