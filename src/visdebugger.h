#ifndef VISDEBUGGER_H_
#define VISDEBUGGER_H_
#include "raylib.h"

void DebugDisplayText(Vector2 pos, int fontSize, Color color,
                      const char *restrict format, ...);

#endif // VISDEBUGGER_H_
