#include "logger.h"


// This is an internal function.
// It is ment to be used through LOG macro, that filles in most of the data
void Logger(const char *restrict inFile, const char *restrict inFunc,
            const int onLine, const enum loglevel loglevel,
            const char *restrict format, ...) {

    if (CURRENT_LOG_LEVEL < loglevel)
        return;

    char messageString[1024] = "";

    va_list argptr;

    int argcount = 0;
    int i = 0;
    char current;

    // Count how many arguments we have
    // by counting format specifiers in a format string
    do {
        current = format[i];
        if (current == '%')
            argcount++;
        i++;
    } while (current != '\0');

    // Pass all the arguments to the vsprinf, which will fill it into the
    // message string. If argptr is 0, then it will just be ignored (hopefully)
    va_start(argptr, argcount);
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
    fprintf(stderr,
            "[%s] %s %s->%s:%d %s\n",
            loglvlToString[loglevel],
            formatedTime,
            inFile,
            inFunc,
            onLine,
            messageString);
}
