// system includes
#include <errno.h>
#include <signal.h>

// local includes
#include "src/asteroid.h"
#include "src/autils.h"
#include "src/benchmarking.h"
#include "src/objecthandler.h"
#include "src/render.h"
#include "src/structs.h"
#include "src/syslogic.h"

// static
#include "src/structs.c"

int main(int argc, char **argv) {
#ifndef BENCHMARKING
    #error "Trying to compile benchmark without BENCHMARKING var!"
#endif

    const float RUN_FOR = 20;

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

    objTracker *tracker = InitTracker();
    GAME_TIME_PASSED    = 0;

    tracker->playerCamera.zoom = .2f;

    tracker->playerCamera.target =
        (Vector2){ WORLD_POS_MAX_X / 2, WORLD_POS_MAX_Y / 2 };
    for (int i = 0; i < SOFT_MAX_ASTEROIDS - 1; i++) {
        AsteroidSafeSpawn(tracker);
    }

    long total_action_list = 0, total_screen_render = 0, total_world_render = 0;
    long total_frames = 0;
    SetTargetFPS(0);

    const float SCREEN_POS_X =
        ((float)SCREEN_WIDTH - (float)(MeasureText("TIME PASSED: 12.34", 24))) /
        2.f;
    while (GAME_TIME_PASSED < RUN_FOR && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        BENCH(RunActionList(tracker), total_action_list);
        BENCH(RunScreenRender(tracker), total_screen_render);
        BENCH(RunWorldRender(tracker), total_world_render);
        DisplayText((Vector2){ SCREEN_POS_X, 30 },
                    24,
                    RAYWHITE,
                    "TIME PASSED: %.2f",
                    GAME_TIME_PASSED);

        DrawFPS(0, 0);
        EndDrawing();
        GAME_TIME_PASSED += GetFrameTime();
        total_frames++;
    }

    printf("\n========== BENCHMARK FINISHED ==========\n");
    printf("Rendered %'ld frames in %'f seconds\n",
           total_frames,
           GAME_TIME_PASSED);

    printf("Average fps: %.0f\n", (float)total_frames / GAME_TIME_PASSED);

    printf("Average frame time: %.2fms\n",
           GAME_TIME_PASSED * 1000.f / (float)total_frames);

    printf("Action list took %'ldus total, %'ldus on average\n",
           total_action_list,
           total_action_list / total_frames);
    printf("Screen render took %'ldus, %'ldus on average\n",
           total_screen_render,
           total_screen_render / total_frames);
    printf("World render took %'ldus, %'ldus on average\n",
           total_world_render,
           total_world_render / total_frames);
    printf("============== END OF LOG ==============\n");
    DeleteTracker(tracker);
    RunCleanup();
    return 0;
}
