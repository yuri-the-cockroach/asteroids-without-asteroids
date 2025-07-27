// system includes
#include <errno.h>
#include <signal.h>

// local includes
#include "src/asteroid.h"
#include "src/autils.h"
#include "src/benchmarking.h"
#include "src/collision.h"
#include "src/gamelogic.h"
#include "src/mt.h"
#include "src/objecthandler.h"
#include "src/render.h"
#include "src/structs.h"
#include "src/syslogic.h"

// static
enum loglevel CURRENT_LOG_LEVEL_CONSOLE = DEFAULT_LOG_LEVEL;
enum loglevel CURRENT_LOG_LEVEL_FILE    = DEFAULT_LOG_LEVEL;

int SCREEN_WIDTH  = 1600;
int SCREEN_HEIGHT = 900;

int FPS_TARGET = 75;

long LAST_SHOT                 = 0;
bool CAMERA_FOLLOW             = true;
float LAST_ASTEROID_SPAWN      = 0;
float NEXT_ASTEROID_SPAWN      = 0;
float GAME_TIME_PASSED         = 0;
enum difficulty CUR_DIFFICULTY = 0;

#ifdef DEBUGGING
objWrap *lastDragged          = NULL;
Vector2 SPEED_PREV            = { 0, 0 };
bool GDB_BREAK                = false;
bool DEBUG_PAUSE              = false;
bool VISUAL_DEBUG_SHOW_POINTS = false;
bool VISUAL_DEBUG             = false;
#endif // DEBUGGING

#ifdef BENCHMARKING
bool BENCHRUNNING        = false;
long BENCH_COLLIDER_TIME = 0;
FILE *BENCH_LOG_FILE_PTR;
#endif // BENCHMARKING

float LAST_FRAME_TIME = 0;

char LOG_FILE_NAME[64];
FILE *LOG_FILE_PTR;

long N_CPU_THREADS = 1;

int main(int argc, char **argv) {

#ifndef BENCHMARKING
    #error "Trying to compile benchmark without BENCHMARKING var!"
#endif

    DEBUG(VISUAL_DEBUG = true);
    const float RUN_FOR = 15;

    errno = 0;

    // Handle the startup arguments
    GetStartUpArguments(argc, argv);
    RunConfig();

    CURRENT_LOG_LEVEL_FILE = CURRENT_LOG_LEVEL_FILE < CURRENT_LOG_LEVEL_CONSOLE
                                 ? CURRENT_LOG_LEVEL_CONSOLE
                                 : CURRENT_LOG_LEVEL_FILE;

    // Handle the interseption of signals
    signal(SIGINT, SigIntHandler);
    signal(SIGTERM, SigIntHandler);
    // signal(SIGSEGV, SigSegvHandel);

    objTracker *tracker = InitTracker();
    GAME_TIME_PASSED    = 0;

    tracker->playerCamera.zoom = .03f;

    tracker->playerCamera.target =
        (Vector2){ WORLD_POS_MAX_X / 2, WORLD_POS_MAX_Y / 2 };

    long total_action_list = 0, total_screen_render = 0, total_world_render = 0;
    long total_mt_collect = 0, total_mt_run = 0;

    long total_frames = 0;

    struct mt_data_wrap *mtDataWrap = InitMT(tracker);
    for (int i = 0; i < SOFT_MAX_ASTEROIDS - 1; i++) {
        SortListByX(tracker);
        if (i % 1000 == 0) { LOG(INFO, "Spawned %d asteroids so far", i); }
        if (!AsteroidSafeSpawn(tracker)) break;
    }

    const float SCREEN_POS_X =
        ((float)SCREEN_WIDTH - (float)(MeasureText("TIME PASSED: 12.34", 24))) /
        2.f;

    FPS_TARGET = 0;
    InitRaylib();
    while (GAME_TIME_PASSED < RUN_FOR && !WindowShouldClose()) {
        // PlayerRuntimeControlls(tracker);
        // Release threads
        BeginDrawing();
        {
            ClearBackground(BLACK);
            CleanupMemory(tracker);
            SortListByX(tracker);
            BENCH(RunThreads(mtDataWrap), total_mt_run);
            BENCH(RunWorldRender(tracker), total_world_render);
            BENCH(RunScreenRender(tracker), total_screen_render);
            // BENCH(RunActionList(tracker), total_action_list);
            BENCH(
                for (unsigned long i = 0; i < tracker->objListLen;
                     i++) { UpdateObjectPos(tracker->objList[i]); },

                total_action_list);

            BENCH(CollectThreads(mtDataWrap), total_mt_collect);
            // Screen renderer

            DisplayText((Vector2){ SCREEN_POS_X, 30 },
                        24,
                        RAYWHITE,
                        "TIME PASSED: %.2f\n"
                        "Objects currently: %d",
                        GAME_TIME_PASSED,
                        tracker->objListLen);

            DrawFPS(0, 0);
        }
        EndDrawing();

        LAST_FRAME_TIME = GetFrameTime();
        GAME_TIME_PASSED += LAST_FRAME_TIME;
        total_frames++;
    }

    BenchLog(
        "=============== BENCHMARK STARTED ==============\n"
        "Item count was %'ld\n"
        "Rendered %'ld frames in %.1f seconds\n"
        "FPS avr: %.0f\n"
        "FRAME TIME avr: %.2fms\n"
        "Action list took %'ldus total, %'ldus on average\n"
        "Mt collect took %'ldus total, %'ldus on average\n"
        "Mt run took %'ldus total, %'ldus on average\n"
        "Screen render took %'ldus, %'ldus on average\n"
        "World render took %'ldus, %'ldus on average\n"
        "============== BENCHMARK FINISHED ==============\n",
        tracker->objListLen,                             // Item count
        total_frames,                                    // Total frames (duh)
        GAME_TIME_PASSED,                                // Runtime
        (float)total_frames / GAME_TIME_PASSED,          // Average FPS
        GAME_TIME_PASSED * 1000.f / (float)total_frames, // Average frame time
        total_action_list,                  // Total run time of action list
        total_action_list / total_frames,   // Average run time of action list
        total_mt_collect,                   // Total run time of mt collect
        total_mt_collect / total_frames,    // Average run time of mt collect
        total_mt_run,                       // Total run time of mt run
        total_mt_run / total_frames,        // Average run time of mt collect
        total_screen_render,                // Total run time of screen render
        total_screen_render / total_frames, // Average run time of screen render
        total_world_render,                 // Total run time of world render
        total_world_render / total_frames); // Average run time of world render

    DeleteTracker(tracker);
    MTCleanupAndFree(mtDataWrap);
    RunCleanup();
    return 0;
}
