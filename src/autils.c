// system includes
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// local includes
#include "autils.h"
#include "structs.h"

int fGetSign(float f) {
    int *ptr = (int *)(void *)&f;
    return *ptr >> 31 | 1;
}

int GetSign(int i) { return i >> 31 | 1; }

float GetRandomFloat(float min, float max) {
    return (float)(GetRandomValue((int)(min * 1000), (int)(max * 1000))) / 1000;
}

// Rollover the number, if it gets over max or under min
float RollOverFloat(float d, float min, float max) {
    const float t = d < min ? max : d;
    return t > max ? min : t;
}

float fCutOff(float f, int n) {
    if (n == 0) return (float)(int)f;
    float mult = powf(10.f, (float)n);
    int temp   = (int)(f * mult);
    return (float)temp / mult;
}

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

void CleanupMemory(objTracker *tracker) {
    unsigned long i = 0;
    unsigned long firstNull = MAX_OBJECT_COUNT;
    unsigned long nullCounter = 0;
    objWrap *current = 0;

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

void RunConfig(void) {
    SetTraceLogLevel(LOG_ERROR);
    setlocale(LC_NUMERIC, "en_US.UTF-8");
#ifdef BENCHMARKING
    BENCH_LOG_FILE_PTR = fopen(BENCH_LOG_FILE_NAME, "w");
    LOG(DEBUG,
        "%s",
        "Compiled with benchmarking support.\nBenchmarking is enabled");
#endif

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "asteroids without asteroids");

    SetWindowPosition(1, 1);
    SetWindowMinSize(640, 480);
    SetWindowMaxSize(8192, 8192);

    SetTargetFPS(FPS_TARGET);
    CreateLogFile();
}

void RunCleanup(void) {
    CloseWindow();
    fclose(LOG_FILE_PTR);

#ifdef BENCHMARKING
    if (BENCH_LOG_FILE_PTR) fclose(BENCH_LOG_FILE_PTR);
#endif
}
