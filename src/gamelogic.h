#ifndef GAMELOGIC_H_
#define GAMELOGIC_H_
#include "structs.h"
#include "objectlogic.h"

void DebugingKeyHandler(ObjectTracker *tracker);
void OnPlayerAccellerate(ObjectStruct *object, float speed);
void KeyboardHandler(ObjectStruct * object);
void ShipControlls(ObjectStruct *object);

#endif // GAMELOGIC_H_
