#include "visdebugger.h"

#pragma GCC diagnostic ignored "-Wvarargs"
#pragma GCC diagnostic ignored "-Wformat-nonliteral"

void DebugDisplayText(Vector2 pos, int fontSize, Color color, const char *restrict format, ...) {
    if (!VISUAL_DEBUG)
        return;

    char messageString[1024] = "";

    va_list argptr;

    int argcount = 0;
    int i = 0;
    char current;

    // Count how many arguments we have
    // by counting format specifiers in a format string
    do {
        current = format[i];
        if (current == '%')
            argcount++;
        i++;
    } while (current != '\0');

    // Pass all the arguments to the vsprinf, which will fill it into the
    // message string. If argptr is 0, then it will just be ignored (hopefully)
    va_start(argptr, argcount);
    vsprintf(messageString, format, argptr);
    va_end(argptr);

    // Actually print to stderr
    DrawText(messageString, (int)pos.x, (int)pos.y, fontSize, color);
}
