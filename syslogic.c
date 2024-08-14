#include "headers/syslogic.h"

void SigIntHandler(int sig) {
    err(SIGINT, "Got a signal interupt from console. Exiting now\n");
}
