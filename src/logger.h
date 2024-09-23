#ifndef LOGGER_H_
#define LOGGER_H_

#define LOG(logLevel, format, ...) Logger(__FILE__, __FUNCTION__, __LINE__, logLevel, format, __VA_ARGS__)

#include "structs.h"

void Logger(const char *restrict inFile, const char *restrict inFunc,
            const int onLine, const enum loglevel loglevel,
            const char *restrict format, ...);

void LoggerToFile(const char *restrict toFile, const char *restrict inFile,
                  const char *restrict inFunc, const char *restrict onLine,
                  const char *restrict msg, const enum loglevel loglevel);

#endif // LOGGER_H_
