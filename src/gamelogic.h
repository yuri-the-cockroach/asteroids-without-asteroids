#ifndef GAMELOGIC_H_
#define GAMELOGIC_H_

#include "structs.h"

int SpawnAsteroidOnTime(objTracker *tracker);
const menuParent *MenuControlls(const menuParent *menu, int *menuHighlighted);
void DebugingKeyHandler(objTracker *tracker);
void OnPlayerAccellerate(object *object, float speed);
void ShipControlls(objTracker *tracker);
void NewGame(objTracker *tracker);
void PlayerRuntimeControlls(objTracker *tracker);

#endif // GAMELOGIC_H_
