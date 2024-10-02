#ifndef SYSLOGIC_H
#define SYSLOGIC_H

#define _XOPEN_SOURCE 700
#define UNW_LOCAL_ONLY

#include <libunwind.h>
#include "structs.h"

void SigIntHandler(int sig);
void SigSegvHandel(int sig);

#endif // SYSLOGIC_H
