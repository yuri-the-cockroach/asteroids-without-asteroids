#include "syslogic.h"

void SigIntHandler(int sig) {
    err(sig, "Got a signal interupt from console. Exiting now\n");
}
