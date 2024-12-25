// system includes
#include <errno.h>
#include <signal.h>

// local includes
#include "src/autils.h"
#include "src/statemachine.h"
#include "src/structs.h"
#include "src/syslogic.h"
#include "src/unit-tests.h"

// static
#include "src/structs.c"

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

    TestCollision();
    fclose(LOG_FILE_PTR);

#ifdef BENCHMARKING
    if (BENCH_LOG_FILE_PTR)
        fclose(BENCH_LOG_FILE_PTR);
#endif
    return 0;
}
