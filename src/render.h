#ifndef RENDER_H_
#define RENDER_H_

#include "structs.h"
#include <stdarg.h>

void DisplayText(Vector2 pos, int fontSize, Color color,
                 const char *restrict format, ...);

// Calls DrawObject on each object in tracker->drawList
// nulls it's position in the list after the object is draw
// and finnaly sets tracker->drawListLen to 0 when everything is done
void DrawAllFromDrawList(objTracker *tracker);

// Draws the object based on it's *object.shape.points* array
void DrawObject(objWrap *wrap);

void DrawGrid2D(int dist, Color color);

void RunScreenRender(objTracker *restrict tracker);
void RunWorldRender(objTracker *restrict tracker);
void RunMenuRender(const menuParent *restrict menu, int menuHighlighted,
                   int subTitleLinesNum, ...);
#endif // RENDER_H_
