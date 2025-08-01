#include "logger.h"
#include "structs.h"

#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
// This is an internal function.
// It is ment to be used through LOG macro, that filles in most of the data
void Logger(const char *restrict inFile, const char *restrict inFunc,
            const int onLine, const enum loglevel loglevel, ...) {

    if (CURRENT_LOG_LEVEL_FILE < loglevel &&
        CURRENT_LOG_LEVEL_CONSOLE < loglevel)
        return;

    char messageString[1024] = "";

    va_list argptr;

    // Count how many arguments we have
    // by counting format specifiers in a format string

    // Pass all the arguments to the vsprinf, which will fill it into the
    // message string. If argptr is 0, then it will just be ignored (hopefully)
    va_start(argptr, 0);

    const char *restrict format = va_arg(argptr, const char *restrict);
    vsnprintf(messageString, 1024, format, argptr);
    va_end(argptr);

    struct timeval time = (struct timeval){ 0, 0 };
    gettimeofday(&time, 0);
    char formatedTime[64] = "";
    snprintf(formatedTime,
             64,
             "%.2ld:%.2ld:%.2ld,%'ld",
             (time.tv_sec / 3600 + 3) % 24,
             time.tv_sec / 60 % 60,
             time.tv_sec % 60,
             time.tv_usec);

    // Actually print to stderr
    if (CURRENT_LOG_LEVEL_CONSOLE >= loglevel)
        fprintf(stderr,
                "%s:%d:0: %s in " TERM_TEXT_MOD(
                    TERM_LIGHT,
                    TERM_WHITE,
                    "%s") " at " TERM_TEXT_MOD(TERM_NORMAL,
                                               TERM_WHITE,
                                               "%s") ": %s\n",
                inFile,
                onLine,
                loglvlToString[loglevel],
                inFunc,
                formatedTime,
                messageString);

    fprintf(LOG_FILE_PTR,
            "%s:%d:0 %s in %s at %s: %s\n",
            inFile,
            onLine,
            loglvlToString[loglevel],
            inFunc,
            formatedTime,
            messageString);
}
