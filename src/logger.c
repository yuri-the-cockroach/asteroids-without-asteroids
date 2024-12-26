#include "logger.h"
#include "structs.h"

// This is an internal function.
// It is ment to be used through LOG macro, that filles in most of the data
void Logger(const char *restrict inFile, const char *restrict inFunc,
            const int onLine, const enum loglevel loglevel, ...) {

    if (CURRENT_LOG_LEVEL_FILE < loglevel &&
        CURRENT_LOG_LEVEL_CONSOLE < loglevel)
        return;

    char messageString[1024] = "";

    va_list argptr;

    FILE *file = LOG_FILE_PTR;
#ifdef BENCHMARKING
    file = loglevel == BENCH ? BENCH_LOG_FILE_PTR : file;
#endif

    // Count how many arguments we have
    // by counting format specifiers in a format string

    // Pass all the arguments to the vsprinf, which will fill it into the
    // message string. If argptr is 0, then it will just be ignored (hopefully)
    va_start(argptr, 0);
    vsprintf(messageString, format, argptr);
    va_end(argptr);

    struct timeval time = (struct timeval){ 0, 0 };
    gettimeofday(&time, 0);
    char formatedTime[64] = "";
    sprintf(formatedTime,
            "%.2ld-%.2ld-%.2ld.%ld",
            (time.tv_sec / 3600 + 3) % 24,
            time.tv_sec / 60 % 60,
            time.tv_sec % 60,
            time.tv_usec);

    // Actually print to stderr
    if (CURRENT_LOG_LEVEL_CONSOLE >= loglevel)
        fprintf(stderr,
                "[%s] %s %s->%s:%d %s\n",
                loglvlToString[loglevel],
                formatedTime,
                inFile,
                inFunc,
                onLine,
                messageString);

    fprintf(file,
            "[%s] %s %s->%s:%d %s\n",
            loglvlToStringNoColor[loglevel],
            formatedTime,
            inFile,
            inFunc,
            onLine,
            messageString);
}
