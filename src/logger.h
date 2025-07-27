#ifndef LOGGER_H_
#define LOGGER_H_
#include "structs.h"

#pragma GCC diagnostic ignored \
    "-Wformat-nonliteral" // Ignore, because format is const char *restrict, so
                          // it's not an issue

#define LOG(logLevel, ...) \
    Logger(__FILE__, __PRETTY_FUNCTION__, __LINE__, logLevel, __VA_ARGS__)

void Logger(const char *restrict inFile, const char *restrict inFunc,
            const int onLine, const enum loglevel loglevel, ...);

#endif // LOGGER_H_
