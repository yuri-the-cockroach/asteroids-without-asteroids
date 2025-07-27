// system includes
#include <errno.h>
#include <signal.h>

// local includes
#include "src/autils.h"
#include "src/statemachine.h"
#include "src/structs.c"
#include "src/syslogic.h"

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
    return 0;
}
