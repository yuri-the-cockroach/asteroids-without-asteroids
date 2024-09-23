#include "logger.h"
#include "structs.h"
#include <stdio.h>
#pragma GCC diagnostic ignored "-Wvarargs"
#pragma GCC diagnostic ignored "-Wformat-nonliteral"

void Logger(const char *restrict inFile, const char *restrict inFunc, const int onLine,
            const enum loglevel loglevel, const char *restrict format, ...) {

    char messageString[1024] = "";

    va_list argptr;

    int argcount = 0;
    int i = 0;
    char current;
    do {
        current = format[i];
        if ( current == '%' ) argcount++;
        i++;
    } while (current != '\0');

    va_start(argptr, argcount);
    vsprintf(messageString, format, argptr);
    va_end(argptr);

    if (CURRENT_LOG_LEVEL < loglevel)
        return;
    fprintf(stderr, "[%s] %s->%s:%d %s\n", loglvlToString[loglevel], inFile, inFunc, onLine, messageString);
}
