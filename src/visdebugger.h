#ifdef DEBUGGING
#ifndef VISDEBUGGER_H_
#define VISDEBUGGER_H_
#include "structs.h"

void DebugDisplayText(Vector2 pos, int fontSize, Color color, const char *restrict format, ...);

#endif // VISDEBUGGER_H_
#endif // DEBUGGING
