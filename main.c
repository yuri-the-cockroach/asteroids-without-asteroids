// system includes
#include <errno.h>
#include <signal.h>

// local includes
#include "src/autils.h"
#include "src/statemachine.h"
#include "src/structs.h"
#include "src/syslogic.h"

enum loglevel CURRENT_LOG_LEVEL_CONSOLE = DEFAULT_LOG_LEVEL;
enum loglevel CURRENT_LOG_LEVEL_FILE = DEFAULT_LOG_LEVEL;

int SCREEN_WIDTH = 1600;
int SCREEN_HEIGHT = 900;

int FPS_TARGET = 75;

long LAST_SHOT = 0;
bool CAMERA_FOLLOW = true;
bool GDB_BREAK = false;
float LAST_ASTEROID_SPAWN = 0;
float NEXT_ASTEROID_SPAWN = 0;
float GAME_TIME_PASSED = 0;
Vector2 SPEED_PREV = {0, 0};

#ifdef DEBUGGING
    bool DEBUG_PAUSE = false;
    bool VISUAL_DEBUG_SHOW_POINTS = false;
    bool VISUAL_DEBUG = false;
#endif // DEBUGGING

#ifdef BENCHMARKING
    bool BENCHRUNNING = false;
    long BENCH_COLLIDER_TIME = 0;
    FILE *BENCH_LOG_FILE_PTR;
#endif // BENCHMARKING

char LOG_FILE_NAME[64];
FILE *LOG_FILE_PTR;

enum difficulty CUR_DIFFICULTY = LOW;

int main(int argc, char **argv) {
    errno = 0;

    // Handle the startup arguments
    RunConfig();
    GetStartUpArguments(argc, argv);

    CURRENT_LOG_LEVEL_FILE = CURRENT_LOG_LEVEL_FILE < CURRENT_LOG_LEVEL_CONSOLE
                                 ? CURRENT_LOG_LEVEL_CONSOLE
                                 : CURRENT_LOG_LEVEL_FILE;

    // Handle the interseption of signals
    signal(SIGINT, SigIntHandler);
    signal(SIGTERM, SigIntHandler);
    // signal(SIGSEGV, SigSegvHandel);

    StateMachine();
    fclose(LOG_FILE_PTR);

    #ifdef BENCHMARKING
    if (BENCH_LOG_FILE_PTR) fclose(BENCH_LOG_FILE_PTR);
    #endif
    return 0;
}
