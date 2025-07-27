// system includes
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// local includes
#include "autils.h"
#include "structs.h"

void CleanupMemory(objTracker *tracker) {
    unsigned long i           = 0;
    unsigned long firstNull   = MAX_OBJECT_COUNT;
    unsigned long nullCounter = 0;
    objWrap *current          = 0;

    while (i < tracker->objListLen) {
        current = tracker->objList[i];
        if (!current) {
            firstNull = i < firstNull ? i : firstNull;
            nullCounter++;
        }
        if (current && firstNull < i) {
            tracker->objList[firstNull] = current;
            tracker->objList[i]         = 0;
            i                           = firstNull;
            nullCounter                 = 0;
            firstNull                   = MAX_OBJECT_COUNT;
        }
        i++;
    }
    tracker->objListLen -= nullCounter;
}

FILE *CreateLogFile(char *restrict file_name_ptr) {
    time_t t           = time(NULL);
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

    snprintf(file_name_ptr,
             LOG_FILE_NAME_SIZE,
             "logs/asteroids-%d-%02d-%02d_%02d-%02d-%02d.log",
             timedate.tm_year + 1900,
             timedate.tm_mon + 1,
             timedate.tm_mday,
             timedate.tm_hour,
             timedate.tm_min,
             timedate.tm_sec);

    FILE *file_ptr = fopen(file_name_ptr, "a");
    errno          = 0;

    return file_ptr;
}

int GetStartUpArguments(int argc, char **argv) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "-lc") ||
                !strcmp(argv[i], "--loglevelconsole")) {
                i++;

                int newLogLevel =
                    ClampInt(atoi(argv[i]), 0, sizeof(loglvlToStringNoColor));
                if (newLogLevel) {
                    CURRENT_LOG_LEVEL_CONSOLE = (enum loglevel)newLogLevel;
                    continue;
                }

                if (!strcmp(argv[i], "0")) {
                    CURRENT_LOG_LEVEL_CONSOLE = NOLOG;
                    continue;
                }
            }

            if (!strcmp(argv[i], "-lf") || !strcmp(argv[i], "--loglevelfile")) {
                i++;

                int newLogLevel =
                    ClampInt(atoi(argv[i]), 0, sizeof(loglvlToStringNoColor));
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
    printf("Current log level set to %d for shell and %d for file\n",
           CURRENT_LOG_LEVEL_CONSOLE,
           CURRENT_LOG_LEVEL_FILE);
    return 0;
}

void RunConfig(void) {
    SetTraceLogLevel(LOG_ERROR);
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    N_CPU_THREADS = sysconf(_SC_NPROCESSORS_ONLN);
    LOG_FILE_PTR  = CreateLogFile(LOG_FILE_NAME);
}


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
