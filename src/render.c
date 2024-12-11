// system includes
#include <raylib.h>
#include <stdlib.h>

// local includes
#include "render.h"
#include "structs.h"
#include "autils.h"


void DisplayText(Vector2 pos, int fontSize, Color color,
                 const char *restrict format, ...) {
    char messageString[1024] = "";

    va_list argptr;

    int argcount = 0;
    UNUSED(argcount);
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

void DrawRectLineNotFucked(int x, int y, int width, int height, Color color) {
    DrawLine(x, y, x + width, y, color);
    DrawLine(x + width, y, x + width, y + height, color);
    DrawLine(x + width, y + height, x, y + height, color);
    DrawLine(x, y + height, x, y, color);
    return;
}

void DrawObject(ObjectWrap *wrap) {

    #ifdef DEBUGGING
    if (VISUAL_DEBUG)
        DrawRectangleLines(
            (int)(wrap->collider.collider.x + wrap->objPtr->position.x),
            (int)(wrap->collider.collider.y + wrap->objPtr->position.y),
            (int)(wrap->collider.collider.width),
            (int)(wrap->collider.collider.height),
            RED);

    DebugDisplayText(
        (Vector2){ wrap->objPtr->position.x, wrap->objPtr->position.y + 50 },
        18,
        WHITE,
        "Heading: %f\nIsCollidable: %d\nSpeed.x: %f\nSpeed.y: %f",
        (double)wrap->objPtr->heading,
        wrap->collider.isCollidable,
        (double)wrap->objPtr->speed.x,
        (double)wrap->objPtr->speed.y);

    if (VISUAL_DEBUG_SHOW_POINTS) {
        DebugDisplayText((Vector2){ wrap->objPtr->position.x,
                                    wrap->objPtr->position.y + 118 },
                         18,
                         WHITE,
                         "Points Pos:");
        for (unsigned long i = 0; i < wrap->objPtr->shape.arrayLength; i++) {

            DebugDisplayText(
                (Vector2){ wrap->objPtr->position.x,
                           wrap->objPtr->position.y + 136 + (float)i * 18 },
                18,
                WHITE,
                "x: %f y: %f",
                wrap->objPtr->shape.points[i].x,
                wrap->objPtr->shape.points[i].y);
        }
    }
    #endif // DEBUGGING

    if (wrap->objPtr->shape.arrayLength == 0) {
        LOG(WARNING, "%s", "Attempting to draw an empty shape");
    }

    unsigned int prevPoint = 0;
    for (unsigned int curPoint = 1; curPoint < wrap->objPtr->shape.arrayLength; curPoint++) {
        DrawLineEx(
            (Vector2){ // Draw from x/y
                       wrap->objPtr->shape.points[prevPoint].x +
                           wrap->objPtr->position.x,
                       wrap->objPtr->shape.points[prevPoint].y +
                           wrap->objPtr->position.y },
            (Vector2){ // Draw to x/y
                       wrap->objPtr->shape.points[curPoint].x +
                           wrap->objPtr->position.x,
                       wrap->objPtr->shape.points[curPoint].y +
                           wrap->objPtr->position.y },
            2.0,  // Thikness of the line
            WHITE // Color of the line
        );
        prevPoint = curPoint;
    }

    // Draw the final line to finish the form. I'm too lazy to do this less
    // janky
    DrawLineEx(
        (Vector2){ // Draw from x/y
                   wrap->objPtr->shape.points[0].x + wrap->objPtr->position.x,
                   wrap->objPtr->shape.points[0].y + wrap->objPtr->position.y },
        (Vector2){
            // Draw to x/y
            wrap->objPtr->shape.points[wrap->objPtr->shape.arrayLength - 1].x +
                wrap->objPtr->position.x,
            wrap->objPtr->shape.points[wrap->objPtr->shape.arrayLength - 1].y +
                wrap->objPtr->position.y },
        1.0,  // Thikness of the line
        WHITE // Color of the line
    );
}

void DrawGrid2D(int dist, Color color) {

    int shiftedCoord = abs(WORLD_POS_MIN_X) + abs(WORLD_POS_MAX_X);

    dist = shiftedCoord % dist != 0 ? dist + (shiftedCoord % dist) : dist;

    for (int current = 0; current < shiftedCoord; current += dist) {
        DrawLine(WORLD_POS_MIN_X + current,
                 WORLD_POS_MIN_Y,
                 WORLD_POS_MIN_X + current,
                 WORLD_POS_MAX_Y,
                 color);
        DrawLine(WORLD_POS_MIN_X,
                 WORLD_POS_MIN_Y + current,
                 WORLD_POS_MAX_X,
                 WORLD_POS_MIN_Y + current,
                 color);
    }

    DrawLine(WORLD_POS_MIN_X,
             WORLD_POS_MIN_Y,
             WORLD_POS_MIN_X,
             WORLD_POS_MAX_Y,
             WHITE);

    DrawLine(WORLD_POS_MIN_X,
             WORLD_POS_MAX_Y,
             WORLD_POS_MAX_X,
             WORLD_POS_MAX_Y,
             WHITE);

    DrawLine(WORLD_POS_MAX_X,
             WORLD_POS_MAX_Y,
             WORLD_POS_MAX_X,
             WORLD_POS_MIN_Y,
             WHITE);

    DrawLine(WORLD_POS_MAX_X,
             WORLD_POS_MIN_Y,
             WORLD_POS_MIN_X,
             WORLD_POS_MIN_Y,
             WHITE);
}

void RunWorldRender(ObjectTracker *tracker) {
    BeginMode2D(tracker->playerCamera);

    DrawRectangle(WORLD_POS_MIN_X,
                  WORLD_POS_MIN_Y,
                  abs(WORLD_POS_MIN_X) + abs(WORLD_POS_MAX_X),
                  abs(WORLD_POS_MIN_Y) + abs(WORLD_POS_MAX_Y),
                  (Color){ 18, 18, 18, 255 });
    DrawGrid2D(200, (Color){ 38, 38, 38, 255 });

    if (tracker->objListLen == 0) {
        LOG(TRACE, "%s", "Object list is empty. Not rendering anything");
        return;
    }

    Vector2 screenStart = {
        tracker->playerCamera.target.x - tracker->playerCamera.offset.x / tracker->playerCamera.zoom,
        tracker->playerCamera.target.y - tracker->playerCamera.offset.y / tracker->playerCamera.zoom
    };

    Vector2 screenEnd = { screenStart.x + (float)SCREEN_WIDTH / tracker->playerCamera.zoom,
                          screenStart.y + (float)SCREEN_HEIGHT / tracker->playerCamera.zoom};

    int DrawingObjCount = 0;
    for (unsigned int i = 0; i < tracker->objListLen; i++) {
        ObjectWrap *current = tracker->objList[i];

        if (!current || !current->draw || current->request != UPDATE)
            continue;

        Vector2 colliderStart = {
            current->objPtr->position.x + current->collider.collider.x,
            current->objPtr->position.y + current->collider.collider.y
        };
        Vector2 colliderEnd = {
            current->objPtr->position.x + current->collider.collider.x + current->collider.collider.width,
            current->objPtr->position.y + current->collider.collider.y + current->collider.collider.height
        };

        if ( colliderEnd.x < screenStart.x ||
             screenEnd.x < colliderStart.x ||
             colliderEnd.y < screenStart.y ||
             screenEnd.y < colliderStart.y )
            continue;

        DrawObject(tracker->objList[i]);

        DrawingObjCount++;
    }
    LOG(TRACE, "Drawing %d objects", DrawingObjCount);

    EndMode2D();
}

void RunScreenRender(ObjectTracker *tracker) {

    SCREEN_WIDTH = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();
    tracker->playerCamera.offset.x = (float)SCREEN_WIDTH / 2;
    tracker->playerCamera.offset.y = (float)SCREEN_HEIGHT / 2;

    DisplayText((Vector2){ 20, 50 },
                24,
                RED,
                "LIVES LEFT: %d",
                tracker->playerPtr->livesLeft);
    DisplayText((Vector2){ 20, 78 },
                20,
                WHITE,
                "PLAYER SCORE: %d",
                tracker->playerScore);

    #ifdef BENCHMARKING
        if (BENCHRUNNING)
            DisplayText((Vector2){ (float)SCREEN_WIDTH / 2 -
                                    (float)(MeasureText("BENCHMARKING", 36)),
                                40 },
                        36,
                        RED,
                        "BENCHMARKING");
    #endif // BENCHMARKING

    #ifdef DEBUGGING
    ObjectStruct *object = tracker->playerPtr->objPtr;

    DebugDisplayText((Vector2){ 20, 110 },
                    18,
                    WHITE,
                    "Acceleration: %f",
                    object->speed.x - SPEED_PREV.x);

    DebugDisplayText((Vector2){ 20, 130 },
                    18,
                    WHITE,
                    "Acceleration: %f",
                    object->speed.y - SPEED_PREV.y);

    DebugDisplayText((Vector2){ 20, 20 },
                    18,
                    WHITE,
                    "Length of the list: %lu",
                    tracker->objListLen);

    DebugDisplayText((Vector2){ 20, 32 },
                    18,
                    WHITE,
                    "Time untill next asteroid: %f",
                    NEXT_ASTEROID_SPAWN - GAME_TIME_PASSED);
    #endif // DEBUGGING
    DrawFPS(0, 0);
}

void RunMenuRender(struct menuParent *menu, const char *restrict title,
                   int subTitleLinesNum, ...) {
    const int titleFontSize = 42;
    const int fontSize = 32;
    int start = SCREEN_HEIGHT / 2 - menu->optionListLen * fontSize / 2;
    int titlePos = 100;

    SCREEN_WIDTH = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();

    DrawRectangle(SCREEN_WIDTH / 3,
                  start / 2,
                  SCREEN_WIDTH / 3,
                  SCREEN_HEIGHT / 2,
                  (Color){ 38, 38, 38, 190 });

    DrawText(title,
             (SCREEN_WIDTH - MeasureText(title, titleFontSize)) / 2,
             titlePos,
             titleFontSize,
             WHITE);

    va_list argptr;
    va_start(argptr, subTitleLinesNum);
    for (int i = 0; i < subTitleLinesNum; i++) {
        char *subtitle = va_arg(argptr, char *);
        DrawText(subtitle,
                 (SCREEN_WIDTH - MeasureText(subtitle, fontSize)) / 2,
                 fontSize * (i + 1) + titlePos + 12,
                 fontSize,
                 WHITE);
    }
    va_end(argptr);

    for (int i = 0; i < menu->optionListLen; i++) {
        if (i == menu->selected) {
            DrawText(menu->optionList[i].name,
                     (SCREEN_WIDTH - MeasureText(menu->optionList[i].name, fontSize)) /
                         2,
                     start + fontSize * i,
                     fontSize,
                     RED);
            continue;
        }

        DrawText(
            menu->optionList[i].name,
            (SCREEN_WIDTH - MeasureText(menu->optionList[i].name, fontSize)) /
                2,
            start + fontSize * i,
            fontSize,
            WHITE);
    }
}
