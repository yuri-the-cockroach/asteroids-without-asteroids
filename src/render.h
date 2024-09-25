#ifndef RENDER_H_
#define RENDER_H_

#include "structs.h"

/* Adds an object to the list for drawing on the next frame. List gets emptied
 * after the frame is drawn */
void AddToDrawList(ObjectTracker *tracker, ObjectWrap *wrap);

/* Calls DrawObject on each object in tracker->drawList
 * nulls it's position in the list after the object is draw
 * and finnaly sets tracker->drawListLen to 0 when everything is done */
void DrawAllFromDrawList(ObjectTracker *tracker);


// Draws the object based on it's *object.shape.points* array
void DrawObject(ObjectWrap *wrap);

void DrawGrid2D( int dist, Color color);
#endif // RENDER_H_
