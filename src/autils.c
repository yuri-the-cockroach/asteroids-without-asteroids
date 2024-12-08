// system includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// local includes
#include "autils.h"
#include "structs.h"

float GetRandomFloat(float min, float max) {
    return (float)(GetRandomValue((int)(min * 1000), (int)(max * 1000))) / 1000;
}

// Rollover the number, if it gets over max or under min
float RollOverFloat(float d, float min, float max) {
    const float t = d < min ? max : d;
    return t > max ? min : t;
}

// Rollover the number, if it gets over max or under min
int RollOverInt(int d, int min, int max) {
    const int t = d < min ? max : d;
    return t > max ? min : t;
}

// Clap the value D from min to max (I hate this code, but it's good in
// assembly)
float ClampFloat(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

int ClampInt(int d, int min, int max) {
    const int t = d < min ? min : d;
    return t > max ? max : t;
}

long GetTimeMicS(void) {
    struct timeval tv = { 0, 0 };
    gettimeofday(&tv, 0);
    return tv.tv_sec * (long)1e6 + tv.tv_usec;
}

void CleanupMemory(ObjectTracker *tracker) {
    unsigned long i = 0;
    unsigned long firstNull = MAX_OBJECT_COUNT;
    unsigned long nullCounter = 0;
    ObjectWrap *current = 0;

    while ( i < tracker->objListLen ) {
        current = tracker->objList[i];
        if ( !current ) {
            firstNull = i < firstNull ? i : firstNull;
            nullCounter++;
        }
        if ( current && firstNull < i) {
            tracker->objList[firstNull] = current;
            tracker->objList[i] = 0;
            i = firstNull;
            nullCounter = 0;
            firstNull = MAX_OBJECT_COUNT;
        }
        i++;
    }
    tracker->objListLen -= nullCounter;
}

int CreateLogFile(void) {
    time_t t = time(NULL);
    struct tm timedate = *localtime(&t);

    struct stat statThingi;
    stat("logs", &statThingi);

    // If logs exists, but is not a directory
    // remove it and set errno to ENOENT
    // So the next condition gets triggered and creates a directory
    if (statThingi.st_mode / 010000 == 010) {
        remove("logs");
        errno = ENOENT;
    }

    if (errno == ENOENT) {
        mkdir("logs", 0755);
        errno = 0;
    }

    sprintf(LOG_FILE_NAME,
            "logs/asteroids-%d-%02d-%02d_%02d-%02d-%02d.log",
            timedate.tm_year + 1900,
            timedate.tm_mon + 1,
            timedate.tm_mday,
            timedate.tm_hour,
            timedate.tm_min,
            timedate.tm_sec);

    LOG_FILE_PTR = fopen(LOG_FILE_NAME, "a");
    errno = 0;

    return 0;
}

int GetStartUpArguments(int argc, char **argv) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "-lc") || !strcmp(argv[i], "--loglevelconsole"))
            {
                i++;

                int newLogLevel = ClampInt(atoi(argv[i]), 0, 8);
                if (newLogLevel) {
                    CURRENT_LOG_LEVEL_CONSOLE = (enum loglevel)newLogLevel;
                    continue;
                }

                if (!strcmp(argv[i], "0")) {
                    CURRENT_LOG_LEVEL_CONSOLE = NOLOG;
                    continue;
                }
            }


            if (!strcmp(argv[i], "-lf") || !strcmp(argv[i], "--loglevelfile"))
            {
                i++;

                int newLogLevel = ClampInt(atoi(argv[i]), 0, 8);
                if (newLogLevel) {
                    CURRENT_LOG_LEVEL_FILE = (enum loglevel)newLogLevel;
                    continue;
                }

                if (!strcmp(argv[i], "0")) {
                    CURRENT_LOG_LEVEL_FILE = NOLOG;
                    continue;
                }
            }
        }
    }
    return 0;
}
