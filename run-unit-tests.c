// system includes
#include <errno.h>
#include <signal.h>

// local includes
#include "src/autils.h"
#include "src/statemachine.h"
#include "src/syslogic.h"

// static
#include "src/structs.c"
#include "src/unit-tests.c"

int main(int argc, char **argv) {
    errno = 0;

    // Handle the startup arguments
    FPS_TARGET = 0;
    RunConfig();
    GetStartUpArguments(argc, argv);

    CURRENT_LOG_LEVEL_FILE = CURRENT_LOG_LEVEL_FILE < CURRENT_LOG_LEVEL_CONSOLE
                                 ? CURRENT_LOG_LEVEL_CONSOLE
                                 : CURRENT_LOG_LEVEL_FILE;

    // Handle the interseption of signals
    signal(SIGINT, SigIntHandler);
    signal(SIGTERM, SigIntHandler);
    // signal(SIGSEGV, SigSegvHandel);

    TestSort();

    InitRaylib();
    const int iter_max = 100;
    for (int i = 0; i < iter_max; i++) {
        TestBigUnit(i, iter_max);
        // TestCollisionHorizontal(i, iter_max);
        // TestCollisionVertical(i, iter_max);
        // TestCollisionAnother(i, iter_max);
        // TestCollisionCos(i, iter_max);
    }
    LOG(INFO, "All tests passed");
    fclose(LOG_FILE_PTR);

#ifdef BENCHMARKING
    if (BENCH_LOG_FILE_PTR)
        fclose(BENCH_LOG_FILE_PTR);
#endif
    return 0;
}
