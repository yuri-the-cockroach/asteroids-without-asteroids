#include "render.h"
#include "structs.h"
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>


void AddToDrawList(ObjectTracker *self, ObjectStruct *obj) {
    if (self->drawListLen >= MAX_OBJECT_COUNT) {
        fprintf(stderr,
                "Error in AddToDrawList, too many objects in "
                "self->drawList\nself->drawListLen is at %ld\n",
                self->drawListLen);
        /* exit(ECHRNG); */
        return;
    }
    self->drawList[self->drawListLen] = obj;
    self->drawListLen++;
}

void DrawAllFromDrawList(ObjectTracker *self) {
    if (self->drawListLen == 0) {
        printf("Warning: Draw list is empty, not drawing anything\n");
        return;
    }

    for (unsigned int i = 0; i < self->drawListLen; i++) {
        DrawObject(self->drawList[i]);
        self->drawList[i] = NULL;
    }
    self->drawListLen = 0;
}

void DrawObject(ObjectStruct *object) {
    int prevPoint = 0;

    if (DEBUG) {
        DrawRectangleLines(object->collider.x + object->position.x,
                      object->collider.y + object->position.y,
                      object->collider.width,
                      object->collider.height,
                      RED);
    }

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
