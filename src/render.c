// system includes
#include "raylib.h"
#include <stdlib.h>

// local includes
#include "autils.h"
#include "render.h"
#include "rlgl.h"
#include "structs.h"
#include "vector-math.h"

#ifdef DEBUGGING
    #include "visdebugger.h"
#endif

static void DrawObject(objWrap *wrap);
static void DrawGrid2D(int dist, Color color);
static void UpdateScreenBorder(objTracker *restrict tracker);

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
        if (current == '%') argcount++;
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

void RunWorldRender(objTracker *restrict tracker) {
    BeginMode2D(tracker->playerCamera);
    UpdateScreenBorder(tracker);

    Vector2 screenStart = tracker->screenBorderWrap.screenStart;
    Vector2 screenEnd   = tracker->screenBorderWrap.screenEnd;

    DrawRectangle(WORLD_POS_MIN_X,
                  WORLD_POS_MIN_Y,
                  fabsf(WORLD_POS_MIN_X) + fabsf(WORLD_POS_MAX_X),
                  fabsf(WORLD_POS_MIN_Y) + fabsf(WORLD_POS_MAX_Y),
                  (Color){ 18, 18, 18, 255 });
    DrawGrid2D(200, (Color){ 38, 38, 38, 255 });

    Vector2 colliderStart, colliderEnd;
    objWrap *current;
    // int DrawingObjCount = 0;
    for (unsigned int i = 0; i < tracker->objListLen; i++) {
        current = tracker->objList[i];

        if (!current || !current->draw || current->request != UPDATE) continue;

        // int DrawingObjCount = 0;
        colliderStart = (Vector2){
            current->objPtr->position.x + current->collider.collider.x,
            current->objPtr->position.y + current->collider.collider.y
        };
        colliderEnd = (Vector2){ current->objPtr->position.x +
                                     current->collider.collider.x +
                                     current->collider.collider.width,
                                 current->objPtr->position.y +
                                     current->collider.collider.y +
                                     current->collider.collider.height };

        if (colliderEnd.x < screenStart.x || screenEnd.x < colliderStart.x ||
            colliderEnd.y < screenStart.y || screenEnd.y < colliderStart.y)
            continue;

        DrawObject(current);

        // DrawingObjCount++;
    }
    // LOG(TRACE, "Drawing %d objects", DrawingObjCount);

// Highlight cursor position
#ifdef DEBUGGING
    if (VISUAL_DEBUG) {
        Vector2 cursorPos =
            (Vector2){ (GetMousePosition().x - tracker->playerCamera.offset.x) /
                               tracker->playerCamera.zoom +
                           tracker->playerCamera.target.x,
                       (GetMousePosition().y - tracker->playerCamera.offset.y) /
                               tracker->playerCamera.zoom +
                           tracker->playerCamera.target.y };
        int rectSize = 20;
        DrawRectangle((int)cursorPos.x - rectSize / 2,
                      (int)cursorPos.y - rectSize / 2,
                      rectSize,
                      rectSize,
                      RED);
    }
#endif
    EndMode2D();
}

void DrawObject(objWrap *restrict wrap) {
    if (!wrap) return;
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
        "Heading: %f\n"
        "IsCollidable: %d\n"
        "Pos.x: %f\n"
        "Pos.y: %f\n"
        "Speed.x: %f\n"
        "Speed.y: %f",
        (double)wrap->objPtr->heading,
        wrap->collider.isCollidable,
        (double)wrap->objPtr->position.x,
        (double)wrap->objPtr->position.y,
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
        return;
    }

    // Texturing is only supported on RL_QUADS
    rlBegin(RL_LINES);
    rlColor4ub(RAYWHITE.r, RAYWHITE.g, RAYWHITE.b, RAYWHITE.a);
    int next;
    Vector2 currentV, nextV;
    nextV = VecAddVec(wrap->objPtr->shape.points[0], wrap->objPtr->position);
    for (unsigned int i = 0; i < wrap->objPtr->shape.arrayLength; i++) {
        next =
            RollOver((int)i + 1, 0, (int)wrap->objPtr->shape.arrayLength - 1);
        currentV = nextV;
        nextV =
            VecAddVec(wrap->objPtr->shape.points[next], wrap->objPtr->position);

        rlVertex2f(currentV.x, currentV.y);
        rlVertex2f(nextV.x, nextV.y);
    }
    rlEnd();
}

void DrawGrid2D(int dist, Color color) {

    int shiftedCoord = fabsf(WORLD_POS_MIN_X) + fabsf(WORLD_POS_MAX_X);

    dist = shiftedCoord % dist != 0 ? dist + (shiftedCoord % dist) : dist;

    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    for (int current = 0; current < shiftedCoord; current += dist) {
        rlVertex2f((float)(WORLD_POS_MIN_X + current), WORLD_POS_MIN_Y);

        rlVertex2f((float)(WORLD_POS_MIN_X + current), WORLD_POS_MAX_Y);
        rlVertex2f(WORLD_POS_MIN_X, (float)(WORLD_POS_MIN_Y + current));
        rlVertex2f(WORLD_POS_MAX_X, (float)(WORLD_POS_MIN_Y + current));
    }
    rlColor4ub(WHITE.r, WHITE.g, WHITE.b, WHITE.a);

    rlVertex2f(WORLD_POS_MIN_X, WORLD_POS_MIN_Y);
    rlVertex2f(WORLD_POS_MIN_X, WORLD_POS_MAX_Y);
    rlVertex2f(WORLD_POS_MIN_X, WORLD_POS_MAX_Y);
    rlVertex2f(WORLD_POS_MAX_X, WORLD_POS_MAX_Y);
    rlVertex2f(WORLD_POS_MAX_X, WORLD_POS_MAX_Y);
    rlVertex2f(WORLD_POS_MAX_X, WORLD_POS_MIN_Y);
    rlVertex2f(WORLD_POS_MAX_X, WORLD_POS_MIN_Y);
    rlVertex2f(WORLD_POS_MIN_X, WORLD_POS_MIN_Y);
    rlEnd();
}

// LOG(TRACE, "Drawing %d objects", DrawingObjCount);

void UpdateScreenBorder(objTracker *tracker) {
    tracker->screenBorderWrap.screenStart = (Vector2){
        tracker->playerCamera.target.x -
            tracker->playerCamera.offset.x / tracker->playerCamera.zoom,
        tracker->playerCamera.target.y -
            tracker->playerCamera.offset.y / tracker->playerCamera.zoom
    };

    tracker->screenBorderWrap.screenEnd =
        (Vector2){ tracker->screenBorderWrap.screenStart.x +
                       (float)SCREEN_WIDTH / tracker->playerCamera.zoom,
                   tracker->screenBorderWrap.screenStart.y +
                       (float)SCREEN_HEIGHT / tracker->playerCamera.zoom };
}

void RunScreenRender(objTracker *restrict tracker) {

    SCREEN_WIDTH                   = GetScreenWidth();
    SCREEN_HEIGHT                  = GetScreenHeight();
    tracker->playerCamera.offset.x = (float)SCREEN_WIDTH / 2;
    tracker->playerCamera.offset.y = (float)SCREEN_HEIGHT / 2;

    if (tracker->playerPtr)
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
    if (tracker->playerPtr) {
        object *object = tracker->playerPtr->objPtr;

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
    }

    DebugDisplayText(
        (Vector2){ 20, 150 }, 18, WHITE, "Cur Difficulty: %d", CUR_DIFFICULTY);

    DebugDisplayText((Vector2){ 20, 20 },
                     18,
                     WHITE,
                     "Length of the list: %lu",
                     tracker->objListLen);

    if (tracker->playerPtr)
        DebugDisplayText((Vector2){ 20, 32 },
                         18,
                         WHITE,
                         "Time untill next asteroid: %f",
                         NEXT_ASTEROID_SPAWN - GAME_TIME_PASSED);
#endif // DEBUGGING
    DrawFPS(0, 0);
}

void RunMenuRender(const menuParent *menu, int menuHighlighted,
                   int subTitleLinesNum, ...) {
    const int titleFontSize = 42;
    const int fontSize      = 32;
    int start    = SCREEN_HEIGHT / 2 - menu->optionListLen * fontSize / 2;
    int titlePos = 100;

    SCREEN_WIDTH  = GetScreenWidth();
    SCREEN_HEIGHT = GetScreenHeight();

    DrawRectangle(SCREEN_WIDTH / 3,
                  SCREEN_HEIGHT / 4,
                  SCREEN_WIDTH / 3,
                  SCREEN_HEIGHT / 2,
                  (Color){ 38, 38, 38, 190 });

    DrawText(menu->name,
             (SCREEN_WIDTH - MeasureText(menu->name, titleFontSize)) / 2,
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
        if (i == menuHighlighted) {
            DrawText(menu->optionList[i].name,
                     (SCREEN_WIDTH -
                      MeasureText(menu->optionList[i].name, fontSize)) /
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
