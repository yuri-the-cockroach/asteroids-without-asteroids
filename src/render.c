#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "render.h"
#include "structs.h"
#include "visdebugger.h"

void AddToDrawList(ObjectTracker *tracker, ObjectWrap *wrap) {
    if (tracker->drawListLen >= MAX_OBJECT_COUNT) {
        LOG(ERROR,
            "%s",
            "Too many objects in tracker->drawList\ntracker->drawListLen"
            "is at %ld");
        return;
    }
    tracker->drawList[tracker->drawListLen] = wrap;
    tracker->drawListLen++;
}

void DrawAllFromDrawList(ObjectTracker *tracker) {
    if (tracker->drawListLen == 0) {
        LOG(DEBUG, "%s", "Drawing list is empty");
        return;
    }

    for (unsigned int i = 0; i < tracker->drawListLen; i++) {
        DrawObject(tracker->drawList[i]);
        tracker->drawList[i] = NULL;
    }
    tracker->drawListLen = 0;
}

void DrawObject(ObjectWrap *wrap) {
    if (VISUAL_DEBUG)
        DrawRectangleLines(
            (int)(wrap->collider.collider.x + wrap->objPtr->position.x),
            (int)(wrap->collider.collider.y + wrap->objPtr->position.y),
            (int)(wrap->collider.collider.width),
            (int)(wrap->collider.collider.height),
            RED);

    DebugDisplayText((Vector2){ wrap->objPtr->position.x, wrap->objPtr->position.y + 50},
                     "Speed.x: %f\nSpeed.y: %f",
                     (double)wrap->objPtr->speed.x,
                     (double)wrap->objPtr->speed.y);

    if (wrap->objPtr->shape.arrayLength == 0) {
        LOG(WARNING, "%s", "Attempting to draw an empty shape");
    }

    unsigned int prevPoint = 0;
    for (unsigned int curPoint = 1; curPoint < wrap->objPtr->shape.arrayLength;
         curPoint++)
    {
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
