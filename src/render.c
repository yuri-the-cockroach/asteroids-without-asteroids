#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>

#include "render.h"
#include "structs.h"
#include "logger.h"

void AddToDrawList(ObjectTracker *tracker, ObjectStruct *obj) {
    if (tracker->drawListLen >= MAX_OBJECT_COUNT) {
        char errmsg[67] = "";
        sprintf(errmsg, "Too many objects in tracker->drawList\ntracker->drawListLen is at %ld", tracker->drawListLen);
        LOG(ERROR, "%s", errmsg);
        return;
    }
    tracker->drawList[tracker->drawListLen] = obj;
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

void DrawObject(ObjectStruct *object) {
    if (CURRENT_LOG_LEVEL >= DEBUG) {
        DrawRectangleLines((int)(object->collider.x + object->position.x),
                      (int)(object->collider.y + object->position.y),
                      (int)(object->collider.width),
                      (int)(object->collider.height),
                      RED);
    }

    if ( object->shape.arrayLength == 0 ) {
        LOG(WARNING, "%s", "Attempting to draw an empty shape");
    }

    unsigned int prevPoint = 0;
    for (unsigned int curPoint = 1; curPoint < object->shape.arrayLength;
         curPoint++)
    {
        DrawLineEx(
            (Vector2){ // Draw from x/y
                       object->shape.points[prevPoint].x + object->position.x,
                       object->shape.points[prevPoint].y + object->position.y },
            (Vector2){ // Draw to x/y
                       object->shape.points[curPoint].x + object->position.x,
                       object->shape.points[curPoint].y + object->position.y },
            2.0,  // Thikness of the line
            WHITE // Color of the line
        );
        prevPoint = curPoint;
    }

    // Draw the final line to finish the form. I'm too lazy to do this less
    // janky
    DrawLineEx(
        (Vector2){ // Draw from x/y
                   object->shape.points[0].x + object->position.x,
                   object->shape.points[0].y + object->position.y },
        (Vector2){ // Draw to x/y
                   object->shape.points[object->shape.arrayLength - 1].x +
                       object->position.x,
                   object->shape.points[object->shape.arrayLength - 1].y +
                       object->position.y },
        1.0,  // Thikness of the line
        WHITE // Color of the line
    );
}
