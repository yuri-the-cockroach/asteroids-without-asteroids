#ifndef GAMELOGIC_H_
#define GAMELOGIC_H_

#include "structs.h"

#include "menulogic.h"
#include "objecthandler.h"
#include "logger.h"
#include "asteroidsutils.h"
#include "objectlogic.h"



void MenuControlls(struct menuParent *menu);
void DebugingKeyHandler(ObjectTracker *tracker);
void OnPlayerAccellerate(ObjectStruct *object, float speed);
void ShipControlls(ObjectTracker *tracker);
void NewGame(ObjectTracker *tracker);
void PlayerRuntimeControlls(ObjectTracker *tracker);

#endif // GAMELOGIC_H_
