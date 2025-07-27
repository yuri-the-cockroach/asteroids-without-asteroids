#include "syslogic.h"
#include "tracingtools.c"
#include <err.h>
#include <signal.h>

void SigIntHandler(int sig) {
    err(sig, "Got a signal interupt from console. Exiting now\n");
}

void SigSegvHandel(int sig) {
    print_stacktrace();
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}
